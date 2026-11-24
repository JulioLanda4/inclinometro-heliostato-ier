#include <Arduino.h>
#include <Wire.h>
#include <TFT_eSPI.h>
#include <Preferences.h>
#include <math.h>

// INCLINOMETRO IER-UNAM | TTGO T-Display ESP32 + BMI160
// 0 grados: espejo vertical | 90 grados: espejo horizontal.
// La gravedad es la referencia. No se calibra el cero al encender.
// Calibracion de 6 caras + referencia geometrica horizontal, guardadas en NVS.
// El instrumento arranca en cualquier posicion despues de calibrarlo una vez.

constexpr uint8_t BMI_ADDR = 0x69;
constexpr int PIN_SDA = 21, PIN_SCL = 22, PIN_LUZ = 4;

// Cambiar solo si la normal del espejo coincide con X o Y.
enum class EjeNormal { X, Y, Z };
constexpr EjeNormal EJE_NORMAL = EjeNormal::Z;

constexpr uint32_t IMU_US = 20000;       // 50 Hz
constexpr uint32_t TFT_MS = 200;         // 5 Hz
constexpr uint32_t SERIAL_MS = 1000;     // 1 Hz
constexpr float TAU_S = 0.35f;
constexpr float ACC_MIN_G = 0.75f, ACC_MAX_G = 1.25f;
constexpr uint32_t FIRMA = 0x494D5536; // "IMU6"

struct Vec3 { float x = 0, y = 0, z = 0; };
struct Muestra { Vec3 a, w; }; // aceleracion g; giro rad/s
struct Calibracion {
  uint32_t firma = FIRMA;
  float bias[3] = {0, 0, 0};
  float ganancia[3] = {1, 1, 1};
};

TFT_eSPI tft;
Preferences prefs;
Calibracion cal;
bool calibracionGuardada = false;
Vec3 gravedad;
Vec3 normalPlano = {0.0f, 0.0f, 1.0f}; // normal nominal o calibrada al espejo
bool planoCalibrado = false;
bool orientacionLista = false;
float angulo = NAN;
uint32_t ultimaIMU = 0, ultimaTFT = 0, ultimoSerial = 0;
int16_t enteroAnterior = -1;

// Datos temporales de las seis caras, sin sobrescribir la
// calibracion actual hasta completar todas las mediciones.
bool enCalibracion = false;
bool caraLista[6] = {false, false, false, false, false, false};
float caraPromedio[6] = {0, 0, 0, 0, 0, 0};
int numeroCaras = 0;

const char* nombresCara[6] = {"+X", "-X", "+Y", "-Y", "+Z", "-Z"};

void pantallaBase(); // usada desde la calibracion

// ---------------------- BMI160 -----------------------------
bool escribirRegistro(uint8_t reg, uint8_t valor) {
  Wire.beginTransmission(BMI_ADDR);
  Wire.write(reg);
  Wire.write(valor);
  return Wire.endTransmission() == 0;
}

bool leerRegistros(uint8_t reg, uint8_t* datos, uint8_t n) {
  Wire.beginTransmission(BMI_ADDR);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) return false;
  if (Wire.requestFrom((uint8_t)BMI_ADDR, n) != n) return false;
  for (uint8_t i = 0; i < n; ++i) datos[i] = Wire.read();
  return true;
}

int16_t aInt16(uint8_t lsb, uint8_t msb) {
  return (int16_t)((uint16_t)lsb | ((uint16_t)msb << 8));
}

bool leerBMI(Muestra& m) {
  uint8_t d[12];
  if (!leerRegistros(0x0C, d, 12)) return false;
  const float factorGiro = DEG_TO_RAD / 131.2f; // +/-250 grados/s
  m.w = {aInt16(d[0], d[1])*factorGiro,
         aInt16(d[2], d[3])*factorGiro,
         aInt16(d[4], d[5])*factorGiro};
  m.a = {aInt16(d[6], d[7])/16384.0f,
         aInt16(d[8], d[9])/16384.0f,
         aInt16(d[10], d[11])/16384.0f}; // +/-2g
  return true;
}

void errorFatal(const char* mensaje) {
  Serial.println(mensaje);
  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.drawString("ERROR", 67, 95, 4);
  tft.drawString(mensaje, 67, 130, 2);
  while (true) delay(1000);
}

void configurarBMI() {
  uint8_t chipID = 0;
  if (!leerRegistros(0x00, &chipID, 1) || chipID != 0xD1)
    errorFatal("BMI160 NO DETECTADO");
  if (!escribirRegistro(0x7E, 0x11)) errorFatal("ACC ERR");
  delay(20);
  if (!escribirRegistro(0x7E, 0x15)) errorFatal("GYR ERR");
  delay(100);
  if (!escribirRegistro(0x40, 0x28) ||
      !escribirRegistro(0x41, 0x03) ||
      !escribirRegistro(0x42, 0x28) ||
      !escribirRegistro(0x43, 0x03))
    errorFatal("CONFIG ERR");
  delay(80);
}

// -------------------- CALIBRACION --------------------------
float componente(const Vec3& v, int eje) {
  return eje == 0 ? v.x : (eje == 1 ? v.y : v.z);
}

float norma(const Vec3& v) {
  return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
}

bool normalizar(Vec3& v) {
  const float n = norma(v);
  if (!isfinite(n) || n < 0.001f) return false;
  v.x /= n; v.y /= n; v.z /= n;
  return true;
}

Vec3 aceleracionCorregida(const Vec3& a) {
  return {(a.x-cal.bias[0])*cal.ganancia[0],
          (a.y-cal.bias[1])*cal.ganancia[1],
          (a.z-cal.bias[2])*cal.ganancia[2]};
}

bool parametrosValidos(const Calibracion& c) {
  if (c.firma != FIRMA) return false;
  for (int i = 0; i < 3; ++i) {
    if (!isfinite(c.bias[i]) || fabsf(c.bias[i]) > 0.20f ||
        !isfinite(c.ganancia[i]) || c.ganancia[i] < 0.85f ||
        c.ganancia[i] > 1.15f) return false;
  }
  return true;
}

void imprimirCalibracion() {
  Serial.printf("Calibracion de 6 caras: %s\n", calibracionGuardada ? "GUARDADA Y ACTIVA" : "SIN CALIBRAR");
  for (int i = 0; i < 3; ++i) {
    Serial.printf("%c: offset=%+.5f g  factor=%.6f\n",
                  'X' + i, cal.bias[i], cal.ganancia[i]);
  }
}

void mostrarAviso(const char* texto1, const char* texto2) {
  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.drawString(texto1, 67, 94, 2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString(texto2, 67, 124, 2);
}

void iniciarCalibracion() {
  enCalibracion = true;
  numeroCaras = 0;
  for (int i = 0; i < 6; ++i) {
    caraLista[i] = false;
    caraPromedio[i] = 0.0f;
  }
  mostrarAviso("CALIBRACION", "0 / 6");
  Serial.println("\n=== CALIBRACION DE 6 CARAS ===");
  Serial.println("Coloca el instrumento apoyado en una cara y dejalo quieto.");
  Serial.println("Envia m + Enter para medir. Cambia de cara y repite 6 veces.");
  Serial.println("Puedes hacerlo en cualquier orden; se identifica el eje solo.");
  Serial.println("Envia x + Enter para cancelar (conserva la calibracion anterior).");
}

void finalizarCalibracion() {
  Calibracion nueva;
  for (int eje = 0; eje < 3; ++eje) {
    float positivo = caraPromedio[2*eje];
    float negativo = caraPromedio[2*eje + 1];
    nueva.bias[eje] = 0.5f*(positivo + negativo);
    nueva.ganancia[eje] = 2.0f/(positivo - negativo);
  }
  if (!parametrosValidos(nueva)) {
    Serial.println("ERROR: parametros fuera de rango; no se guardo nada.");
    enCalibracion = false;
    pantallaBase();
    ultimaIMU = micros();
    return;
  }
  size_t escritos = prefs.putBytes("cal6", &nueva, sizeof(nueva));
  if (escritos != sizeof(nueva)) {
    Serial.println("ERROR al guardar en memoria; se conserva la calibracion anterior.");
    enCalibracion = false;
    pantallaBase();
    ultimaIMU = micros();
    return;
  }
  cal = nueva;
  calibracionGuardada = true;
  // La normal guardada dependia de la calibracion anterior.
  prefs.remove("planoH");
  normalPlano = {0.0f, 0.0f, 1.0f};
  planoCalibrado = false;
  orientacionLista = false; // reiniciar el filtro con datos corregidos
  enCalibracion = false;
  Serial.println("\nCALIBRACION COMPLETA. Guardada en memoria del ESP32.");
  imprimirCalibracion();
  mostrarAviso("CALIBRADO", "6 / 6 OK");
  delay(750);
  pantallaBase();
  ultimaIMU = micros();
}

void capturarCara() {
  if (!enCalibracion) return;
  Serial.println("Midiendo durante 2 segundos; NO MOVER...");

  constexpr int N = 200;
  Vec3 suma, suma2;
  int validas = 0;
  float maxGiro = 0.0f;

  for (int i = 0; i < N; ++i) {
    Muestra m;
    if (leerBMI(m)) {
      suma.x += m.a.x; suma.y += m.a.y; suma.z += m.a.z;
      suma2.x += m.a.x*m.a.x;
      suma2.y += m.a.y*m.a.y;
      suma2.z += m.a.z*m.a.z;
      maxGiro = fmaxf(maxGiro, norma(m.w)*RAD_TO_DEG);
      ++validas;
    }
    delay(10);
  }
  if (validas < 190) {
    Serial.println("RECHAZADA: error I2C. Repite la toma.");
    return;
  }
  Vec3 media = {suma.x/validas, suma.y/validas, suma.z/validas};
  Vec3 sigma = {
    sqrtf(fmaxf(0, suma2.x/validas - media.x*media.x)),
    sqrtf(fmaxf(0, suma2.y/validas - media.y*media.y)),
    sqrtf(fmaxf(0, suma2.z/validas - media.z*media.z))
  };
  float magnitud = norma(media);
  Serial.printf("Promedio: X=%+.4f Y=%+.4f Z=%+.4f g | norma=%.4f g\n",
                media.x, media.y, media.z, magnitud);
  if (magnitud < 0.85f || magnitud > 1.15f ||
      sigma.x > 0.02f || sigma.y > 0.02f || sigma.z > 0.02f ||
      maxGiro > 3.0f) {
    Serial.println("RECHAZADA: hubo movimiento, vibracion o aceleracion anomala.");
    return;
  }

  int eje = 0;
  for (int i = 1; i < 3; ++i)
    if (fabsf(componente(media, i)) > fabsf(componente(media, eje))) eje = i;
  float principal = componente(media, eje);
  if (fabsf(principal) < 0.88f) {
    Serial.println("RECHAZADA: alinea mejor una cara con la horizontal.");
    return;
  }
  int posicion = 2*eje + (principal < 0 ? 1 : 0);
  if (caraLista[posicion]) {
    Serial.printf("REPETIDA: cara %s. Gira el dispositivo a una cara distinta.\n",
                  nombresCara[posicion]);
    return;
  }
  caraLista[posicion] = true;
  caraPromedio[posicion] = principal;
  ++numeroCaras;
  Serial.printf("ACEPTADA: %s (%d/6).\n", nombresCara[posicion], numeroCaras);
  Serial.print("Faltan: ");
  for (int i = 0; i < 6; ++i)
    if (!caraLista[i]) { Serial.print(nombresCara[i]); Serial.print(' '); }
  Serial.println();

  char progreso[16];
  snprintf(progreso, sizeof(progreso), "%d / 6", numeroCaras);
  mostrarAviso("CALIBRACION", progreso);
  if (numeroCaras == 6) finalizarCalibracion();
}

// ------------ REFERENCIA GEOMETRICA HORIZONTAL --------------
// Captura la direccion de gravedad con el instrumento apoyado
// sobre su cara paralela al espejo, en una superficie nivelada.
// Esta direccion expresada en ejes del sensor es la normal real
// del espejo. No es un cero temporal; se guarda permanentemente.
void calibrarPlanoHorizontal() {
  if (enCalibracion) {
    Serial.println("Termina o cancela antes la calibracion de 6 caras.");
    return;
  }

  mostrarAviso("HORIZONTAL", "NO MOVER");
  Serial.println("\nCalibracion geometrica: superficie horizontal nivelada.");
  Serial.println("Tomando 200 muestras durante ~2 segundos. No mover...");

  constexpr int N = 200;
  Vec3 suma, suma2;
  float giroMax = 0.0f;
  int validas = 0;
  for (int i = 0; i < N; ++i) {
    Muestra m;
    if (leerBMI(m)) {
      Vec3 a = aceleracionCorregida(m.a);
      suma.x += a.x; suma.y += a.y; suma.z += a.z;
      suma2.x += a.x*a.x; suma2.y += a.y*a.y; suma2.z += a.z*a.z;
      giroMax = fmaxf(giroMax, norma(m.w)*RAD_TO_DEG);
      ++validas;
    }
    delay(10);
  }

  // Rechazar una captura incorrecta conservando la referencia previa.
  if (validas < 190) {
    Serial.println("RECHAZADA: lecturas I2C insuficientes.");
    pantallaBase(); ultimaIMU = micros();
    return;
  }
  Vec3 media = {suma.x/validas, suma.y/validas, suma.z/validas};
  Vec3 sigma = {
    sqrtf(fmaxf(0.0f, suma2.x/validas-media.x*media.x)),
    sqrtf(fmaxf(0.0f, suma2.y/validas-media.y*media.y)),
    sqrtf(fmaxf(0.0f, suma2.z/validas-media.z*media.z))
  };
  const float magnitud = norma(media);
  if (magnitud < 0.90f || magnitud > 1.10f ||
      sigma.x > 0.02f || sigma.y > 0.02f || sigma.z > 0.02f ||
      giroMax > 3.0f || fabsf(media.z/magnitud) < 0.95f) {
    Serial.println("RECHAZADA: mueve menos el dispositivo y comprueba la horizontalidad.");
    pantallaBase(); ultimaIMU = micros();
    return;
  }
  Vec3 nueva = media;
  if (!normalizar(nueva)) {
    Serial.println("RECHAZADA: vector de gravedad invalido.");
    pantallaBase(); ultimaIMU = micros();
    return;
  }

  if (prefs.putBytes("planoH", &nueva, sizeof(nueva)) != sizeof(nueva)) {
    Serial.println("ERROR: no se pudo guardar la referencia en NVS.");
    pantallaBase(); ultimaIMU = micros();
    return;
  }
  normalPlano = nueva;
  planoCalibrado = true;
  orientacionLista = false;
  Serial.printf("Referencia horizontal GUARDADA: n=(%+.6f, %+.6f, %+.6f)\n",
                normalPlano.x, normalPlano.y, normalPlano.z);
  mostrarAviso("CALIBRADO", "90 GRADOS");
  delay(700);
  pantallaBase();
  ultimaIMU = micros();
}

void borrarCalibracion() {
  prefs.remove("cal6");
  prefs.remove("planoH");
  normalPlano = {0.0f, 0.0f, 1.0f};
  planoCalibrado = false;
  cal = Calibracion();
  calibracionGuardada = false;
  orientacionLista = false;
  Serial.println("Calibracion borrada. Se usa el acelerometro sin correcciones.");
  pantallaBase();
}

void procesarSerial() {
  while (Serial.available()) {
    char c = (char)Serial.read();
    if (c == '\n' || c == '\r' || c == ' ') continue;
    switch (c) {
      case 'c': case 'C': iniciarCalibracion(); break;
      case 'm': case 'M':
        if (enCalibracion) capturarCara();
        else Serial.println("Primero envia c para iniciar calibracion.");
        break;
      case 'x': case 'X':
        enCalibracion = false;
        pantallaBase();
        ultimaIMU = micros();
        Serial.println("Calibracion cancelada. Se conserva la anterior.");
        break;
      case 'h': case 'H':
        calibrarPlanoHorizontal();
        break;
      case 'p': case 'P':
        imprimirCalibracion();
        Serial.printf("Referencia horizontal: %s | normal=(%+.5f,%+.5f,%+.5f)\n",
                      planoCalibrado ? "GUARDADA" : "NOMINAL (0,0,1)",
                      normalPlano.x, normalPlano.y, normalPlano.z);
        break;
      case 'b': case 'B':
        if (!enCalibracion) borrarCalibracion();
        else Serial.println("Cancela con x antes de borrar.");
        break;
      default: Serial.println("Comandos: c=6 caras, m=medir, h=horizontal, x=cancelar, p=ver, b=borrar");
    }
  }
}

// ------------------- ORIENTACION ---------------------------
void actualizarOrientacion(Muestra m, float dt) {
  m.a = aceleracionCorregida(m.a);
  const float modulo = norma(m.a);
  if (!isfinite(modulo) || modulo < 0.001f) return;
  Vec3 aNorm = m.a;
  if (!normalizar(aNorm)) return;

  if (!orientacionLista) {
    gravedad = aNorm;
    orientacionLista = true;
    return;
  }
  Vec3 g = gravedad;
  Vec3 pred = {
    g.x - (m.w.y*g.z - m.w.z*g.y)*dt,
    g.y - (m.w.z*g.x - m.w.x*g.z)*dt,
    g.z - (m.w.x*g.y - m.w.y*g.x)*dt
  };
  if (!normalizar(pred)) return;
  if (modulo >= ACC_MIN_G && modulo <= ACC_MAX_G) {
    float alpha = expf(-dt/TAU_S);
    pred.x = alpha*pred.x + (1-alpha)*aNorm.x;
    pred.y = alpha*pred.y + (1-alpha)*aNorm.y;
    pred.z = alpha*pred.z + (1-alpha)*aNorm.z;
    if (!normalizar(pred)) return;
  }
  gravedad = pred;
}

float calcularElevacion() {
  if (!orientacionLista) return NAN;
  // Producto escalar entre normal fisica y gravedad (ambos unitarios).
  const float producto = normalPlano.x*gravedad.x +
                         normalPlano.y*gravedad.y +
                         normalPlano.z*gravedad.z;
  const float c = constrain(fabsf(producto), 0.0f, 1.0f);
  // atan2 es numericamente robusto cerca de 90 grados.
  return atan2f(c, sqrtf(fmaxf(0.0f, 1.0f-c*c)))*RAD_TO_DEG;
}

// --------------------- TFT -------------------------------
void pantallaBase() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(1);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.drawString("ELEVACION", 67, 12, 2);
  tft.drawFastHLine(5, 25, 125, TFT_DARKGREY);
  enteroAnterior = -1;
}

constexpr uint8_t SA=1, SB=2, SC=4, SD=8, SE=16, SF=32, SG=64;
void dibujarDigito(uint8_t n, int16_t x) {
  static const uint8_t pat[10] = {
    SA|SB|SC|SD|SE|SF, SB|SC, SA|SB|SG|SE|SD,
    SA|SB|SC|SD|SG, SF|SG|SB|SC,
    SA|SF|SG|SC|SD, SA|SF|SE|SD|SC|SG,
    SA|SB|SC, SA|SB|SC|SD|SE|SF|SG,
    SA|SB|SC|SD|SF|SG
  };
  if (n>9) return;
  uint8_t p=pat[n];
  const int16_t y=31,w=56,h=149,t=10,ym=y+(h-t)/2;
  if (p&SA) tft.fillRoundRect(x+t,y,w-2*t,t,4,TFT_WHITE);
  if (p&SB) tft.fillRoundRect(x+w-t,y+t,t,ym-y-t,4,TFT_WHITE);
  if (p&SC) tft.fillRoundRect(x+w-t,ym+t,t,y+h-t-(ym+t),4,TFT_WHITE);
  if (p&SD) tft.fillRoundRect(x+t,y+h-t,w-2*t,t,4,TFT_WHITE);
  if (p&SE) tft.fillRoundRect(x,ym+t,t,y+h-t-(ym+t),4,TFT_WHITE);
  if (p&SF) tft.fillRoundRect(x,y+t,t,ym-y-t,4,TFT_WHITE);
  if (p&SG) tft.fillRoundRect(x+t,ym,w-2*t,t,4,TFT_WHITE);
}

void mostrarAngulo() {
  if (!isfinite(angulo)) return;
  int centesimas = constrain((int)lroundf(angulo*100),0,9000);
  int entero = centesimas/100, decimales = centesimas%100;
  if (entero!=enteroAnterior) {
    tft.fillRect(0,27,135,160,TFT_BLACK);
    if (entero>=10) {
      const int16_t x=(135-(2*56+9))/2;
      dibujarDigito(entero/10,x);
      dibujarDigito(entero%10,x+65);
    } else dibujarDigito(entero,(135-56)/2);
    enteroAnterior=entero;
  }
  tft.fillRect(0,188,135,52,TFT_BLACK);
  char fraccion[8];
  snprintf(fraccion,sizeof(fraccion),".%02d",decimales);
  tft.setTextDatum(MC_DATUM);
  tft.setTextSize(1);
  tft.setTextColor(TFT_CYAN,TFT_BLACK);
  tft.drawString(fraccion,59,209,4);
  tft.drawCircle(116,198,5,TFT_CYAN);
  tft.drawCircle(116,198,4,TFT_CYAN);
}

void setup() {
  Serial.begin(115200);
  delay(350);
  tft.init();
  tft.setRotation(0); // VERTICAL 135x240
  pinMode(PIN_LUZ,OUTPUT);
  digitalWrite(PIN_LUZ,HIGH);
  pantallaBase();

  Wire.begin(PIN_SDA,PIN_SCL);
  Wire.setClock(400000);
  configurarBMI();

  prefs.begin("imu-ier",false);
  Calibracion guardada;
  if (prefs.getBytesLength("cal6")==sizeof(guardada) &&
      prefs.getBytes("cal6",&guardada,sizeof(guardada))==sizeof(guardada) &&
      parametrosValidos(guardada)) {
    cal=guardada;
    calibracionGuardada = true;
    Serial.println("Calibracion de 6 caras cargada desde memoria.");
  } else {
    Serial.println("Aun no hay calibracion: usando valores por defecto.");
  }

  // Cargar calibracion geometrica, si ya se guardo con 'h'.
  if (prefs.getBytesLength("planoH") == sizeof(Vec3)) {
    Vec3 guardadaNormal;
    if (prefs.getBytes("planoH", &guardadaNormal, sizeof(guardadaNormal)) == sizeof(guardadaNormal) &&
        normalizar(guardadaNormal) && fabsf(guardadaNormal.z) >= 0.95f) {
      normalPlano = guardadaNormal;
      planoCalibrado = true;
      Serial.println("Referencia geometrica horizontal cargada desde memoria.");
    }
  }

  Muestra primero;
  if (!leerBMI(primero)) errorFatal("SIN DATOS IMU");
  actualizarOrientacion(primero,0.0f);
  angulo=calcularElevacion();
  ultimaIMU=micros();
  ultimaTFT=ultimoSerial=millis();
  mostrarAngulo();
  Serial.println("0 vertical | 90 horizontal | sin cero manual.");
  Serial.println("Comandos (enviar y pulsar Enter):");
  Serial.println("c=6 caras, m=medir, h=referencia horizontal, x=cancelar, p=ver, b=borrar");
  imprimirCalibracion();
  Serial.printf("Referencia horizontal: %s | n=(%+.5f, %+.5f, %+.5f)\n",
                planoCalibrado ? "GUARDADA" : "NOMINAL",
                normalPlano.x, normalPlano.y, normalPlano.z);
}

void loop() {
  procesarSerial();
  if (enCalibracion) return;
  uint32_t ahora=micros();
  uint32_t lapso=ahora-ultimaIMU;
  if (lapso<IMU_US) return;
  ultimaIMU=ahora;
  float dt=lapso/1000000.0f;
  if (dt>0.10f) dt=IMU_US/1000000.0f;
  Muestra m;
  if (!leerBMI(m)) return;
  actualizarOrientacion(m,dt);
  angulo=calcularElevacion();
  if (millis()-ultimaTFT>=TFT_MS) {
    ultimaTFT=millis();
    mostrarAngulo();
  }
  if (millis()-ultimoSerial>=SERIAL_MS) {
    ultimoSerial=millis();
    Vec3 a=aceleracionCorregida(m.a);
    Serial.printf("Angulo: %.2f | a[g] X=%+.3f Y=%+.3f Z=%+.3f\n",
                  angulo,a.x,a.y,a.z);
  }
}
