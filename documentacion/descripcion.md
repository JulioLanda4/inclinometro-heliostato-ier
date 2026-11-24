# Descripción y alcance

## Motivación

El mini horno solar del IER-UNAM utiliza un helióstato con movimiento de elevación y acimut. Durante el trabajo de diagnóstico del sistema de movimiento surgió la necesidad de contar con una medición **independiente de los encoders** para verificar la inclinación física del espejo y contrastarla con el ángulo indicado por el controlador.

## Objetivo

Desarrollar y evaluar un instrumento autónomo, portátil y de bajo costo para determinar el ángulo físico de inclinación de la superficie del espejo, mediante un BMI160 y una TTGO T-Display ESP32.

## Alcance

El instrumento mide **inclinación** respecto a la gravedad. No mide el ángulo de acimut ni la posición angular absoluta alrededor de la vertical. Tampoco calcula la altura solar astronómica. No forma parte aún de un lazo de control automático validado: su uso inicial es diagnóstico y verificación experimental.

## Convención

Se define la inclinación θ del plano del espejo: 0° en posición vertical y 90° en posición horizontal. La relación con el ángulo de elevación utilizado por otros programas debe verificarse caso por caso antes de comparar lecturas.

## Estado

Firmware con lectura IMU, estimación gravitatoria, filtro complementario, pantalla y dos métodos de calibración. Faltan caracterización metrológica, documentación eléctrica formal y archivos CAD originales de la carcasa.
