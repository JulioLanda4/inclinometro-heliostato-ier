# Carcasa impresa en 3D

Se incorporan **los archivos STEP y STL suministrados por el autor del prototipo**. No son reconstrucciones ni modelos aproximados. Se incluyen, además, los enlaces aportados por el autor a los documentos originales en Onshape. La comparación dimensional con el prototipo físico sigue pendiente.

## Contenido

| Pieza | Geometría CAD | Para impresión | Dimensiones del modelo (aprox.) |
| --- | --- | --- | --- |
| Caja | [Caja.step](step/Caja.step) | [Caja.stl](stl/Caja.stl) | 80 × 35 × 28 mm |
| Tapa | [Tapa.step](step/Tapa.step) | [Tapa.stl](stl/Tapa.stl) | 80 × 35 × 2 mm |
| Soporte | [Soporte.step](step/Soporte.step) | [Soporte.stl](stl/Soporte.stl) | 35 × 4,45 × 1,5 mm |
| Conjunto | [Ensamblaje.step](step/Ensamblaje.step) | No se adjuntó STL del conjunto | 80 × 35 × 30 mm |

Se verificó que el conjunto STEP contiene **tres sólidos** y que los STL individuales son mallas cerradas. Las dimensiones se obtuvieron de la geometría exportada; deben contrastarse con medidas físicas y tolerancias de la impresión.

## Diseño original en Onshape

La carcasa se diseñó en **Onshape**. En esta plataforma, el modelo nativo y su historial de operaciones se almacenan en la nube; no es necesario adjuntar un archivo «nativo» local para referenciarlo. Los enlaces originales proporcionados por el autor son:

| Elemento | Documento original |
| --- | --- |
| Ensamblaje | [Abrir en Onshape](https://cad.onshape.com/documents/e81be42059bb55de0d709128/w/c6dc132a05e3a671168342b6/e/75cb94c4598dea076f06c29f?renderMode=0&uiState=6b04df4e14881e4dbd61388f) |
| Tapa | [Abrir en Onshape](https://cad.onshape.com/documents/e5cd4f27ac69ba1d77e22c3b/w/3132c3af8d63e0d613293178/e/f54b7971a95a501d9688879b?renderMode=0&uiState=6b04df95d084f790d6fb1ff3) |
| Soporte | [Abrir en Onshape](https://cad.onshape.com/documents/5d1779da3b4f8078b25876ed/w/638948b83df320bc65df28aa/e/57400e19f17936508644280e?renderMode=0&uiState=6b04dfb28311d34b25c0d4c7) |
| Caja | [Abrir en Onshape](https://cad.onshape.com/documents/e81be42059bb55de0d709128/w/c6dc132a05e3a671168342b6/e/f0020ba9f0425d817adcd140?renderMode=0&uiState=6b04dfcd6d0f573e5df0ae25) |

**Acceso:** los enlaces están registrados, pero **no se ha podido verificar el acceso público sin sesión**. Antes de hacer público este repositorio, comprobar los permisos de cada documento con una ventana privada del navegador y autorizar visualización o exportación según corresponda. No se debe compartir acceso de edición involuntariamente.

## ¿Qué formato utilizar?

- **STEP (`step/`)**: para examinar la geometría sólida e importarla a otros programas CAD. No necesariamente conserva el historial paramétrico de operaciones.
- **STL (`stl/`)**: para laminar e imprimir cada pieza. Comprobar las unidades y orientación antes de imprimir.
- **Onshape**: referencia al diseño fuente; sus herramientas de edición y su historial de operaciones dependen de los permisos del documento.

## Impresión y ensamblaje

Las piezas se ofrecen como modelos de referencia. **La elección de material, orientación de impresión, altura de capa, relleno, soportes y demás ajustes queda a criterio de quien fabrique el dispositivo**, según su impresora y el uso previsto. No se establece un perfil de laminado obligatorio ni se afirma haber validado uno.

Antes del montaje se recomienda comprobar el ajuste de las piezas, la posición de la placa BMI160, la cara geométrica de referencia y su paralelismo con el espejo. Las fotografías de montaje permiten identificar la disposición general, pero no reemplazan un control dimensional.

## Aspectos aún por documentar

- Registrar tornillería, insertos roscados, imanes y método de fijación, cuando se verifiquen sus especificaciones.
- Comprobar el paralelismo de la superficie de apoyo de la carcasa con el plano del espejo y la estabilidad mecánica del conjunto.
- Verificar tolerancias y el alojamiento de batería, cables, interruptor y JST de seis pines frente a la versión física ensamblada.

**Nota:** la presencia de los archivos permite reproducir la geometría exportada, pero todavía no certifica tolerancias, resistencia mecánica ni calidad de impresión.
