# Arquitectura del firmware

## Entorno

- Plataforma: Espressif32.
- Placa: `lilygo-t-display`.
- Framework: Arduino mediante PlatformIO.
- Bibliotecas: `Wire`, `TFT_eSPI`, `Preferences`.
- Archivo principal: `src/main.cpp`.

## Flujo lógico

```text
Inicio
  ├─ Inicializar ST7789 y bus I²C
  ├─ Comprobar identidad BMI160 (0xD1)
  ├─ Configurar acelerómetro y giroscopio
  ├─ Leer calibraciones desde NVS
  └─ Inicializar vertical estimada a partir de aceleración
       ↓
Bucle (~50 Hz)
  ├─ Escuchar comandos serie
  ├─ Leer aceleración y velocidad angular
  ├─ Corregir aceleraciones (seis caras)
  ├─ Actualizar vector de gravedad filtrado
  ├─ Calcular elevación con normal geométrica
  ├─ Actualizar TFT (~5 Hz)
  └─ Enviar datos serie (~1 Hz)
```

## Parámetros del código

| Parámetro | Valor nominal |
| --- | --- |
| Dirección I²C | `0x69` |
| Muestreo en programa | 50 Hz |
| Configuración salida BMI160 | 100 Hz |
| Actualización TFT | 5 Hz |
| Reporte serial | 1 Hz |
| Monitor serie | 115200 baudios |
| Posición vertical | 0° |
| Posición horizontal | 90° |

## Almacenamiento no volátil

El firmware usa el espacio `imu-ier` de `Preferences` (NVS); guarda los parámetros de seis caras bajo `cal6` y la normal geométrica bajo `planoH`. Repetir la calibración de seis posiciones elimina la referencia geométrica guardada; debe repetirse `h`.

## Cambios futuros propuestos (no implementados)

- Separar lectura del BMI160, calibraciones, filtro y dibujo en módulos independientes.
- Registrar muestras en CSV con tiempo para análisis de estabilidad y error.
- Añadir diagnóstico del sensor ante fallos I²C persistentes.
- Documentar prueba de compilación automatizada antes de una versión estable.

**Nota:** la fuente incluida es la última copia de trabajo disponible para preparar el repositorio; debe compararse contra la versión realmente usada en las pruebas antes de declararla liberación de firmware.
