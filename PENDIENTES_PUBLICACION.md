# Verificaciones antes de publicar

- [ ] Comparar `src/main.cpp` con la versión exacta que está instalada y funciona en la TTGO.
- [ ] Ejecutar `pio run` localmente y corregir cualquier diferencia de entorno.
- [ ] Verificar que los parámetros `platformio.ini` correspondan a la placa real.
- [ ] Decidir titularidad y licencia de firmware y documentación.
- [ ] Revisar imágenes y materiales de terceros antes de publicarlos.
- [x] Registrar conexión declarada del interruptor entre RST/EN y GND (sin corte real de batería).
- [x] Incorporar fotografías del montaje y rectificar la capacidad nominal de batería a 1000 mAh.
- [ ] Confirmar físicamente el circuito del cargador LiPo, ruta de batería y RESET.
- [ ] Identificar serie/paso y comprobar el pinout real del conector JST de seis posiciones.
- [ ] Recuperar o redibujar correctamente el esquemático.
- [x] Incorporar STEP y STL reales de caja, tapa y soporte, más ensamblaje STEP.
- [x] Registrar los enlaces a los cuatro documentos originales de Onshape (ensamblaje, caja, tapa y soporte).
- [ ] Verificar que los cuatro enlaces de Onshape tengan los permisos de visualización previstos para el público.
- [ ] Verificar ajuste y tolerancias de las piezas en un nuevo montaje; los parámetros de impresión quedan a criterio de cada fabricante.
- [ ] Validar con referencias angulares independientes.
- [ ] Publicar la primera etiqueta de versión solo tras los puntos pertinentes.
