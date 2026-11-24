# Publicar el repositorio desde macOS y VS Code

## 1. Preservar tu código funcional

Antes de reemplazar nada, conserva una copia del proyecto que ya está funcionando en tu Mac. **Compara** `src/main.cpp` y `platformio.ini` con los archivos de tu proyecto local. Si difieren, usa como fuente de verdad la versión probada físicamente, e integra después la documentación de esta carpeta.

## 2. Crear repositorio vacío en GitHub

La cuenta autenticada en el conector de GitHub durante la preparación del paquete fue `JulioLanda4`. Desde el navegador, crea un repositorio nuevo con nombre `inclinometro-heliostato-ier`, preferentemente **privado** mientras se revisan código, derechos y contenidos. No marques la opción de inicializar README, .gitignore o licencia, porque estos archivos ya existen localmente.

Página de creación: https://github.com/new

## 3. Iniciar Git dentro de la carpeta del proyecto

Abre esta carpeta en VS Code y ejecuta en su terminal:

```bash
git init -b main
git add .
git commit -m "Inicio del repositorio: firmware y documentación inicial"
git remote add origin https://github.com/JulioLanda4/inclinometro-heliostato-ier.git
git push -u origin main
```

Si Git solicita credenciales o permisos, autentícate con el método habitual de tu instalación (por ejemplo, GitHub CLI o el flujo de VS Code). Si `git` ya está inicializado, no repitas el comando de inicialización y revisa los remotos con `git remote -v`.

## 4. Verificar

- Comprobar que GitHub muestra los directorios esperados.
- Confirmar que la compilación automática termina correctamente (si el repositorio tiene Actions habilitadas).
- Evitar etiquetar como estable el firmware hasta cotejarlo con la copia que realmente has usado.

## 5. Material no disponible

El esquemático electrónico formal **todavía no se incluye**. La carcasa sí contiene STEP y STL, además de los enlaces a los cuatro modelos originales de Onshape. Comprobar la configuración de acceso de esos documentos antes de publicar el repositorio.
