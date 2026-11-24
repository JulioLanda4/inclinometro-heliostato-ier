# Manual de uso del inclinómetro

## Antes de encender

- Confirmar que la carcasa está rígidamente fijada a la superficie que se quiere medir.
- No asumir que la cara exterior utilizada para la calibración es paralela al espejo sin comprobar su montaje.
- Comprobar que el circuito de alimentación y la batería no presentan daño.

## Interruptor del prototipo

En la versión construida, el interruptor está conectado entre **RST/EN y GND**. Al cerrarlo mantiene al ESP32 en reinicio, pero **no separa físicamente la batería de la placa**. No debe interpretarse como un corte completo de energía ni como una solución de almacenamiento prolongado de la batería.

## Operación habitual

1. Enciende el inclinómetro en **cualquier orientación**.
2. La pantalla vertical mostrará la inclinación del plano: 0° vertical; 90° horizontal.
3. Espera a que la lectura se estabilice antes de anotarla.
4. Evita movimientos bruscos, vibraciones y aceleraciones mientras tomas mediciones de precisión.

No hace falta calibrar el cero al iniciar. Los parámetros de seis caras y de orientación geométrica se recuperan de NVS.

## Para calibrar desde VS Code

Conecta mediante USB-C, abre el monitor serie de PlatformIO a 115200 baudios:

| Comando | Significado |
| --- | --- |
| `c` | Iniciar seis caras |
| `m` | Capturar la cara actual |
| `h` | Guardar una referencia horizontal conocida |
| `p` | Ver estado de calibración |
| `x` | Cancelar |
| `b` | Borrar ambas calibraciones |

Consulta el [procedimiento completo](calibracion.md).

## Si PlatformIO no puede cargar el programa

La placa original tiene un botón de arranque **BOOT/GPIO0**. Es posible poner el microcontrolador en modo descarga manualmente; dependiendo de la conexión del RESET externo, se puede mantener BOOT, activar RESET y luego iniciar la carga. Antes de repetir, confirmar que el puerto USB detectado corresponde a la TTGO.

## Errores conocidos

- Un valor de 88–89° en una mesa horizontal puede indicar desalineación de montaje, no necesariamente fallo del BMI160.
- La lectura puede cambiar momentáneamente durante un giro rápido; dejarla estabilizar.
- Si cambian las conexiones o el montaje, revisar el firmware y recalibrar.
