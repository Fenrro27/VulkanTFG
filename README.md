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
2. **vcpkg**: Gestor de paquetes configurado con la variable de entorno `VCPKG_ROOT`.
3. **CMake**: Versión 3.20 o superior.
4. **Compilador**:
   - **Windows**: Visual Studio 2022.
   - **Linux/macOS**: GCC, Clang o AppleClang.

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

#### macOS
```bash
# Configurar
cmake --preset macos-debug

# Compilar y Ejecutar
cmake --build --preset macos-debug --target run
```

---

## Estructura del Proyecto
- `ProjectTFG/`: Código fuente del motor gráfico.
- `shaders/`: Código fuente de los GLSL shaders.
- `textures/`: Texturas del proyecto.
- `models/`: Modelos 3D (OBJ/TinyObjLoader).
- `out/`: Directorio de salida generado automáticamente (ignorado por Git).

## Notas de Desarrollo
Para añadir nuevos archivos al proyecto, simplemente inclúyelos en la carpeta `ProjectTFG/` y CMake los detectará automáticamente en la siguiente compilación.
