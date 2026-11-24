# Esquemático electrónico — en preparación

Todavía no hay un `.kicad_sch` verificado. La primera versión de GitHub contiene las conexiones I²C documentadas y fotografías del prototipo.

## Componentes a incluir

1. TTGO T-Display ESP32 original (módulo de desarrollo).
2. Breakout BMI160 (representar el módulo y sus pines externos, no necesariamente el chip desnudo).
3. Batería LiPo 3,7 V y **1000 mAh** (etiqueta del prototipo).
4. Interruptor SPST **entre RST/EN y GND**. Este circuito **no desconecta físicamente la batería**.
5. Conector JST externo **de seis posiciones**, con pinout todavía **pendiente de comprobación**.
6. Pulsador RESET externo, **si se confirma como componente separado en el montaje**.

## Criterio para publicar el esquema

- Verificar por continuidad los terminales de todos los componentes antes de fijar el pinout.
- Verificar sentido, serie y paso del JST, señalando la posición del pin 1.
- Documentar tensión real y ruta de conexión de la batería a la placa.
- Guardar `.kicad_pro`, `.kicad_sch` y, si aplican, bibliotecas personalizadas, además de una exportación PDF o SVG.
- Conservar derechos y atribución de símbolos/huellas de terceros.

**No publicar como construido un cableado basado únicamente en propuestas previas.**
