# Plan de validación metrológica (pendiente de ejecutar)

## Propósito

Evaluar experimentalmente error sistemático, repetibilidad, histéresis, estabilidad y autonomía. Los resultados se incorporarán **únicamente después de realizar mediciones con una referencia angular independiente**.

## Ensayo de exactitud y repetibilidad

1. Utilizar un instrumento angular de referencia cuyo desempeño sea conocido.
2. Medir en 0°, 15°, 30°, 45°, 60°, 75° y 90°.
3. Registrar al menos 5 repeticiones por posición, en recorridos ascendentes y descendentes, si la mecánica lo permite.
4. Esperar a que el inclinómetro se estabilice y anotar temperatura y condiciones relevantes.
5. Registrar la lectura mostrada y la referencia en [plantilla CSV](../datos/validacion/plantilla-mediciones.csv).

Para cada muestra, \(e_i=	heta_{medida,i}-	heta_{ref,i}\). Evaluar:

\[
ME=rac1N\sum_i e_i,\quad MAE=rac1N\sum_i|e_i|,\quad RMSE=\sqrt{rac1N\sum_i e_i^2}.
\]

Presentar también la dispersión por posición y el contraste entre sentido de subida y bajada. La incertidumbre de referencia y montaje deberá discutirse explícitamente.

## Estabilidad

Registrar durante al menos 15–30 minutos sobre una base inmóvil; comprobar cambios por calentamiento, ruido y el posible efecto de integrar el giroscopio.

## Autonomía y alimentación

Registrar condiciones de carga, tensión, consumo y duración efectiva con batería. No inferir autonomía únicamente a partir de 250 mAh nominales.

## Resultados disponibles

**Aún no se dispone de una campaña de validación completa.** Los datos usados para motivar la calibración geométrica fueron mediciones exploratorias, no un conjunto de validación independiente.
