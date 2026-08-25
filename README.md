# ProjectTFG - Motor Gráfico Vulkan

Este proyecto consiste en un motor gráfico de alto rendimiento desarrollado en C++ utilizando la API de **Vulkan**. Está diseñado como un sistema portátil y modular, capaz de ejecutarse en Windows, Linux y macOS.

## Características principales
- **API Gráfica**: Vulkan SDK.
- **Gestión de Dependencias**: vcpkg para una gestión automatizada y portable.
- **Sistema de Construcción**: CMake con Presets modernos.
- **Sombreadores (Shaders)**: Compilación automática de archivos `.vert`, `.frag` y `.comp` a formato SPIR-V.
- **Recursos**: Carga automática de texturas, modelos 3D y sistemas de partículas.

---

## Requisitos Previos
1. **Vulkan SDK**: Instalado y en el PATH del sistema.
2. **vcpkg**: Gestor de paquetes configurado con la variable de entorno `VCPKG_ROOT` (o instalado en visual studio).
> **⚠️ Importante:** La versión de vcpkg debe ser posterior al **18 de octubre de 2023** para que reconozca el campo `license` en las dependencias modernas (como ImGui).
> Si se va a usar Visual Studio 2022 o superior debe instalar el vcpkg en visual studio.
> * Tu historial local de vcpkg debe estar lo suficientemente actualizado para contener el commit del **baseline exigido por este proyecto** (`d80785c4ee1df22a4119a3451452f59e978e06aa`).
3. **CMake**: Versión 3.20 o superior.
4. **Compilador**:
   - **Windows**: Visual Studio 2022.
   - **Linux/macOS**: GCC, Clang o AppleClang.
> **Entorno probado:** El proyecto ha sido desarrollado y validado utilizando **Visual Studio Community 2022 (v17.14.32)** y **vcpkg (versión 2026-04-08)**.
---

## Instrucciones de Compilación y Ejecución

### 1. Configuración inicial (Solo la primera vez)
Define la ruta de tu instalación de vcpkg:
- **Windows (PowerShell)**: `$env:VCPKG_ROOT = "C:\ruta\a\vcpkg"`
- **Linux/macOS (Bash)**: `export VCPKG_ROOT=/ruta/a/vcpkg`

### 2. Comandos por Plataforma

#### Windows
```powershell
# Configurar
cmake --preset win-debug

# Compilar y Ejecutar (Todo en uno)
cmake --build --preset win-debug --target run
```

#### Linux
```bash
# Configurar
cmake --preset linux-debug

# Compilar y Ejecutar
cmake --build --preset linux-debug --target run
```

#### macOS (Aún no compatible)
```bash
# Configurar
cmake --preset macos-debug

# Compilar y Ejecutar
cmake --build --preset macos-debug --target run
```

---

## Estructura del Proyecto
El código fuente está organizado en subcarpetas por módulo (layout estándar de motores gráficos Vulkan). Los includes entre clases son planos (`#include "GEGraphicsContext.h"`) y se resuelven automáticamente porque el CMake añade todas las subcarpetas a las rutas de inclusión.

```
ProjectTFG/
├── main.cpp              # Entry point (GEApplication::run)
├── resource.h            # IDs de recursos (shaders SPIR-V embebidos)
├── Core/                 # App, escena, cámara, skybox (GEApplication, GEScene, GECamera, GESkybox, GEWindowPosition, commonDebug)
├── Render/               # Infraestructura Vulkan: contexto, swapchain, render pass, pipelines, depth (GEGraphicsContext, GEDrawingContext, GECommandContext, GERenderingContext, GEDepthBuffer)
├── Resources/            # Recursos GPU: texturas, buffers, descriptor sets, pipeline config (GETexture, GEVertexBuffer, GEIndexBuffer, GEUniformBuffer, GEDescriptorSet, GEPipelineConfig)
├── Geometry/             # Figuras paramétricas (GEFigure + 9 subclases, GEGround)
├── Objects/              # Objetos compuestos y modelos OBJ (GEObject, GEPiece, GEModel)
├── Particles/            # Sistemas de partículas y compute shaders (GEParticlesSystem, GEHumo, GEFuego, GEAgua, GEComputeShader, GEParticleBuffer, GEParticle, GEEmitterParams)
├── Types/                # Structs GPU compartidos (GEVertex, GETransform, GEMaterial, GELight)
├── shaders/              # Código fuente de los GLSL shaders
├── textures/             # Texturas del proyecto
└── models/               # Modelos 3D (OBJ/TinyObjLoader)
```
- `out/`: Directorio de salida generado automáticamente (ignorado por Git).
- Para añadir código nuevo: crea el `.cpp`/`.h` en la subcarpeta correspondiente y CMake lo detecta solo (`GLOB_RECURSE ... CONFIGURE_DEPENDS`); no hay que tocar el CMakeLists.

## Solución de Problemas Frecuentes
`Error`: unexpected field 'license', did you mean 'supports'?
Este error ocurre cuando se utiliza una versión anticuada de la herramienta vcpkg que no soporta características modernas introducidas a finales de 2023.

- `Solución`: Ve a tu instalación local de vcpkg (C:\vcpkg o similar), actualiza el repositorio con git pull y recompila la herramienta ejecutando .\bootstrap-vcpkg.bat. Si estás compilando desde Visual Studio en lugar de la terminal, asegúrate de actualizar el IDE a su última versión.

## Notas de Desarrollo
Para añadir nuevos archivos al proyecto, simplemente inclúyelos en la carpeta `ProjectTFG/` y CMake los detectará automáticamente en la siguiente compilación.
