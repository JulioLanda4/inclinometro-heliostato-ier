# Calibración del inclinómetro

El equipo usa la gravedad como referencia. **No necesita establecer un cero cada vez que se enciende.** Existen dos calibraciones diferentes: eléctrica del acelerómetro y geométrica del montaje.

## Preparación

1. Conecta la TTGO a la computadora con un cable de datos.
2. Abre el monitor serie de PlatformIO a **115200 baudios** y pulsa Enter al enviar comandos.
3. Evita vibraciones, desplazamientos y cambios bruscos de temperatura durante la toma de muestras.

## Paso A — Seis caras

1. Envía `c` para iniciar.
2. Coloca el equipo de forma que un eje del BMI160 quede aproximadamente alineado con la gravedad.
3. Mantén inmóvil y envía `m`. El programa registra unas 200 lecturas, identifica el eje predominante y muestra la cara aceptada.
4. Repite hasta completar `+X`, `−X`, `+Y`, `−Y`, `+Z`, `−Z`. Si una cara se repite o el movimiento es excesivo, la captura se rechaza.
5. Solo después de completar las seis posiciones se calculan y almacenan las correcciones de offset y sensibilidad.
6. Envía `p` para revisar `bias` y `ganancia`.

La calibración previa no se sobrescribe al comenzar un nuevo ciclo. El comando `x` cancela. Si el sensor no puede apoyarse correctamente sobre sus seis caras, es preferible adaptar el procedimiento y documentar referencias angulares confiables antes que forzar las medidas.

## Paso B — Referencia geométrica horizontal

**Requisito:** la cara que se apoya en la mesa debe ser paralela al plano del espejo en el montaje definitivo.

1. Coloca esa cara del dispositivo sobre una mesa correctamente nivelada.
2. Sin moverlo, envía `h`.
3. El programa toma unas 200 muestras del acelerómetro **ya corregidas**, determina una normal de referencia y la guarda en NVS.
4. Envía `p` y confirma que la referencia horizontal está `GUARDADA`.
5. Reinicia el equipo; verifica que la referencia se recupere de memoria.

Durante el desarrollo se observó una lectura alrededor de **88,8°** con la carcasa sobre una mesa nivelada, asociada a componentes del acelerómetro próximas a **X=+0,015 g**, **Y=−0,015 g** y **Z=+1,000 g**. Se implementó esta corrección para compensar un desfase geométrico aparente cercano a 1,2°; la causa física exacta no se ha aislado metrológicamente.

## Repetición y conservación

- Si cambia la carcasa, se desmonta el BMI160 o cambia la superficie de referencia, repetir `h`.
- Si cambian los parámetros de seis caras, repetir también `h`.
- `b` borra ambas calibraciones; no ejecutarlo salvo que se desee iniciar nuevamente.
- Evitar publicar volcados de memoria NVS como si fueran datos de validación: son parámetros particulares del instrumento.

## Precaución

Que el equipo marque 90° en la superficie usada como referencia no prueba exactitud en 0°, 30°, 45°, 60° o 75°. La calibración debe verificarse con mediciones independientes, según [Validación](validacion.md).
