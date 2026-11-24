# Historial de desarrollo y decisiones

Este registro resume decisiones identificadas en las conversaciones del proyecto. No pretende sustituir un historial de commits que no existía durante las primeras pruebas.

## Etapa 1 — Motivación

En el mini horno solar del IER-UNAM se observó la necesidad de comprobar la elevación física del helióstato independientemente de la lectura de los encoders, tras problemas de discordancia angular entre el sistema de mando y el movimiento real.

## Etapa 2 — Selección del hardware

Se consideraron varias opciones; se eligió una **TTGO T-Display ESP32** con **BMI160**, batería LiPo y carcasa impresa en 3D. La posibilidad de medir también el acimut con un sensor que incluya magnetómetro quedó fuera del alcance final.

## Etapa 3 — Montaje y comunicación

Se ensambló el prototipo y se comprobaron tensiones y alimentación. En PlatformIO se detectó el puerto USB y se resolvió el procedimiento de carga de firmware. El escaneo I²C identificó un dispositivo en **0x69**; el firmware verificó el identificador **0xD1** del BMI160.

## Etapa 4 — Estimación angular

Primero se mostraron Pitch y Roll. Se determinó que Pitch correspondía al movimiento de elevación en el montaje ensayado. Tras descartar un acimut fiable con el BMI160, se fijó un único objetivo: inclinación física del espejo con **0° vertical y 90° horizontal**.

## Etapa 5 — Referencia por gravedad

Se reemplazó el cero relativo al encender por una medición absoluta basada en la dirección de gravedad, con filtro complementario vectorial. Se mantuvo la pantalla ST7789 en vertical y la prioridad visual de dígitos grandes.

## Etapa 6 — Calibración

Se implementó calibración de seis posiciones para corrección de offset y ganancia. En una mesa nivelada se observaron ~88,8° en lugar de 90°, junto con aceleraciones X e Y próximas a ±0,015 g. Posteriormente se añadió una referencia geométrica horizontal guardada en NVS. Se informó que el instrumento volvió a comportarse correctamente, pendiente de validación metrológica.

## Próximos pasos

Elaborar esquemático contrastado, incorporar CAD/STL original, realizar ensayos de exactitud e incertidumbre y preparar una publicación de firmware con compilación reproducible y licencia definida.
