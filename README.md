# Sistema de Detección de Alertas

Un sistema en tiempo real para detectar, rastrear y generar alertas basadas en la detección de objetos utilizando OpenCV y Qt. Esta aplicación procesa las transmisiones en vivo de cámaras, detecta objetos utilizando algoritmos como HOG o Haar Cascade, y gestiona alertas para los objetos detectados basadas en su comportamiento a lo largo del tiempo.

## Características

- **Detección de Objetos en Tiempo Real**: Soporta la detección de peatones utilizando HOG o rostros utilizando Haar Cascade.
- **Soporte Multicámara**: Procesa las transmisiones de varias cámaras simultáneamente.
- **Sistema de Alertas**: Rastrea los objetos detectados y activa alertas basadas en condiciones específicas.
- **Captura de Imágenes**: Guarda imágenes de objetos que activan alertas para su posterior revisión.
- **Opciones de Ordenamiento**: Ordena las alertas por tiempo, fecha o ID de la cámara.

## Dependencias

Como el proyecto se encuentra en desarrollo, para construir y ejecutar el proyecto, necesitas:

- **Qt Framework** (versión 6 o posterior)
- **OpenCV** (versión 4.1 o posterior)
- Un compilador C++ compatible con tu versión de Qt

Asegúrate de que los siguientes módulos de OpenCV estén instalados:

- `opencv_core`
- `opencv_highgui`
- `opencv_imgcodecs`
- `opencv_imgproc`
- `opencv_feautures2d`
- `opencv_calib3d`
- `opencv_videoio`
- `opencv_objdetect`

## Construcción del Proyecto

###  Paso 1: Clonar el Repositorio:
   ```bash
   git clone https://github.com/Santie-a/proyecto-algoritmos.git
   cd sistema-deteccion-alertas
   ```

### Paso 2: Abrir el Proyecto en Qt Creator

1. Lanza **Qt Creator**.
2. Abre el archivo `.pro` que se encuentra en el directorio raíz del proyecto.

### Paso 3: Configurar la Construcción

1. Selecciona tu versión de Qt y el compilador adecuado para tu sistema.
2. Configura el proyecto como una construcción **Release** o **Debug**, según prefieras.

### Paso 4: Construir y Ejecutar el Proyecto

1. Haz clic en **Build All** para construir el proyecto.
2. Una vez construido, haz clic en **Run** para ejecutar el proyecto.