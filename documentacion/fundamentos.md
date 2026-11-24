# Fundamento de la medición

## Vector gravitatorio

Cuando el acelerómetro permanece prácticamente inmóvil, su medida de fuerza específica permite estimar la **dirección vertical** en el sistema de ejes del sensor, salvo el signo, irrelevante aquí porque usamos un valor absoluto.

Sean las aceleraciones corregidas \(a_x,a_y,a_z\). Se normalizan como

\[
\hat{\mathbf g}=\frac{(a_x,a_y,a_z)}{\sqrt{a_x^2+a_y^2+a_z^2}}.
\]

La **normal unitaria de referencia** del plano del espejo, expresada en ejes del sensor, se representa por \(\hat{\mathbf n}\). La inclinación del plano respecto de la vertical (0° vertical, 90° horizontal) se obtiene mediante

\[
\boxed{\theta=\arcsin\!\left(\left|\hat{\mathbf n}\cdot\hat{\mathbf g}\right|\right)}.
\]

Esta definición no distingue de qué lado de la vertical se encuentra el plano; el valor se limita al intervalo [0°, 90°]. La normal de referencia viene inicialmente dada por el eje Z del módulo y luego puede sustituirse mediante la **calibración geométrica** sobre una superficie horizontal.

## Calibración del acelerómetro

Para cada eje \(i\), la calibración de seis posiciones estima las lecturas medias \(a_i^+\) y \(a_i^-\) al colocar el eje en sentidos opuestos respecto a la gravedad. Se calcula:

\[
b_i=\frac{a_i^++a_i^-}{2}, \qquad s_i=\frac{2}{a_i^+-a_i^-}.
\]

La corrección aplicada es \(a_i^{corr}=s_i(a_i-b_i)\). Este ajuste individual por eje no corrige por sí solo la no ortogonalidad entre ejes ni una desalineación de montaje.

## Filtrado complementario vectorial

El firmware actual transporta la estimación de vertical con las velocidades angulares del giroscopio:

\[
\mathbf g_{pred}\simeq\mathbf g_{prev}-\left(\boldsymbol\omega\times\mathbf g_{prev}\right)\Delta t,
\]

normaliza el vector y lo aproxima al vector acelerométrico a través de una combinación ponderada, con \(\alpha=\exp(-\Delta t/\tau)\), usando \(\tau=0.35\,s\) en esta versión.

## Límites

- Aceleración dinámica y vibración pueden confundirse parcialmente con gravedad.
- El giro presenta sesgo y deriva, por lo que la gravedad debe corregir la estimación de manera continua.
- Una referencia horizontal calibrada no demuestra la exactitud en los 90° de recorrido.
- La resolución de pantalla de 0,01° **no es una incertidumbre medida**.
- Un cambio en la posición de la carcasa respecto al espejo invalida la calibración geométrica.

Más detalles de registro y medidas en [Calibración](calibracion.md) y [Validación](validacion.md).
