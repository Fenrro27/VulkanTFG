# AGENTS.md

## Proyecto
Motor gráfico de alto rendimiento en **C++17** con **Vulkan**, multiplataforma (Windows/Linux/macOS). Incluye: skybox, modelos OBJ, figuras paramétricas, iluminación Phong, partículas simuladas con compute shaders y UI ImGui.

- Gestión de dependencias: **vcpkg** (baseline `d80785c4ee1df22a4119a3451452f59e978e06aa`).
- Sistema de build: **CMake** con presets (`CMakePresets.json`).
- Shaders compilados a SPIR-V y embebidos como recursos (ver `Project6.rc` / `resource.h`).
- Código y comentarios en **español**.

## Comandos
```bash
# Configurar
cmake --preset linux-debug          # o win-debug / macos-debug

# Compilar y ejecutar
cmake --build --preset linux-debug --target run

# Solo compilar
cmake --build --preset linux-debug
```
- Shaders: `ProjectTFG/shaders/CompileShaders.bat` (Windows; glslc a SPIR-V).
- Documentación: `doxygen Doxyfile`.
- Los `.cpp`/`.h` nuevos en cualquier subcarpeta de `ProjectTFG/` se detectan solos por CMake (`GLOB_RECURSE ... CONFIGURE_DEPENDS`); los includes planos (`#include "GEGraphicsContext.h"`) se resuelven porque todas las subcarpetas están en `target_include_directories`.

## Estructura
```
VulkanTFG/
├── CMakePresets.json        # Presets linux/win/macos (-debug)
├── vcpkg.json               # Dependencias vcpkg
├── Doxyfile
├── ProjectTFG/              # Todo el código fuente del motor
│   ├── main.cpp             # Entry point (GEApplication::run)
│   ├── resource.h           # IDs de recursos (shaders SPIR-V embebidos)
│   ├── Core/                # App, escena, cámara, skybox (GEApplication, GEScene, GECamera, GESkybox, GEWindowPosition, commonDebug)
│   ├── Render/              # Infraestructura Vulkan (GEGraphicsContext, GEDrawingContext, GECommandContext, GERenderingContext, GEDepthBuffer)
│   ├── Resources/           # Recursos GPU (GETexture, GEVertexBuffer, GEIndexBuffer, GEUniformBuffer, GEDescriptorSet, GEPipelineConfig)
│   ├── Geometry/            # Figuras paramétricas (GEFigure + 9 subclases, GEGround)
│   ├── Objects/             # Objetos compuestos y modelos OBJ (GEObject, GEPiece, GEModel)
│   ├── Particles/           # Partículas y compute (GEParticlesSystem, GEHumo/Fuego/Agua, GEComputeShader, GEParticleBuffer, GEParticle, GEEmitterParams)
│   ├── Types/               # Structs GPU compartidos (GEVertex, GETransform, GEMaterial, GELight)
│   ├── shaders/             # GLSL (.vert/.frag/.comp)
│   ├── textures/            # Imágenes (wood, moon, fire, smoke, pngwing...)
│   └── models/              # OBJ (fontain, campfire, newFountain, train)
├── Memoria/                 # Documentación TFG
└── out/                     # Salida de build (ignorado)
```

## Mapa de clases
| Subsistema | Clase | Rol |
|---|---|---|
| Núcleo | `GEGraphicsContext` | Instancia, device físico/lógico, `graphicsQueueMutex` |
| Núcleo | `GEDrawingContext` | Swapchain, semáforos/fences, submit + present |
| Núcleo | `GECommandContext` | Command buffers por imagen |
| Núcleo | `GERenderingContext` | Render pass, pipelines gráficos, framebuffers, depth |
| Recursos | `GETexture` | Textura 2D y cubemap (staging → imagen → sampler) |
| Recursos | `GEVertexBuffer` / `GEIndexBuffer` / `GEUniformBuffer` | Buffers de vértices, índices y uniformes |
| Recursos | `GEDescriptorSet` | Descriptor sets por frame |
| Recursos | `GEPipelineConfig` | Configuración declarativa de pipeline |
| Figuras | `GEFigure` (base) | Malla paramétrica con materiales/luz |
| Figuras | `GESphere` `GECube` `GECone` `GECylinder` `GEPyramid` `GEDisk` `GEIcosahedron` `GETorus` | Subclases con constructor paramétrico |
| Figuras | `GEGround` | Suelo (subclase de `GEFigure`) |
| Objetos | `GEObject` / `GEPiece` | Objeto compuesto por piezas |
| Objetos | `GEModel` | Carga de `.obj` con TinyObjLoader |
| Cámara | `GECamera` | Modos FREE/FPS/OBSERVING, `addObservationPoint` |
| Partículas | `GEParticlesSystem` | Doble buffer GPU (pboA/pboB ping-pong) |
| Partículas | `GEHumo` `GEFuego` `GEAgua` | Subclases del sistema de partículas |
| Partículas | `GEComputeShader` | Pipeline compute + dispatch por sistema |
| Escena | `GEScene` | Contiene escena completa + carga en hilo |
| App | `GEApplication` | Ventana GLFW, bucle principal, ImGui, pantalla de carga |
| Otros | `GESkybox` | Cubemap de cielo |

## Flujos clave
- **Arranque**: `main.cpp` → `GEApplication::run()` → init GLFW → `GEGraphicsContext` (crea instancia/dispositivo) → `GEDrawingContext` (swapchain) → `GECommandContext` → `GERenderingContext` (pipelines) → `GEScene` → `loadingLoop()` lanza `loaderThread` con `GEScene::loadAssets()` → `mainLoop()`.
- **Carga en segundo plano**: el constructor de `GEScene` es ligero; los assets pesados se cargan en `loadAssets()` (hilo separado) con progreso vía `setLoadProgress()` protegido por `loadMutex`.
- **Render loop** (`GEApplication::mainLoop` → `draw`): espera imagen → `recordComputeCommands` (física+compute, doble buffer) → `drawGraphicsObjects` (render pass, skybox → figuras → objetos → partículas) → `submitGraphicsCommands` → `submitPresentCommands`.
- **Física**: paso fijo (`updatePhysics`) + interpolación `alpha` en `draw`.
- **Partículas**: `GEComputeShader::recordCommands` despacha el shader compute (grupos de 256) que lee `pboA` y escribe `pboB`; el dibujo usa el buffer de salida con barreras entre pasos.

## Modelo de hilos
- **`loaderThread`**: carga la escena en segundo plano mientras se pinta la pantalla de carga.
- **`graphicsQueueMutex`** (en `GEGraphicsContext`): serializa el acceso a la cola de gráficos. Se bloquea con `std::lock_guard` en `GEDrawingContext::submitGraphicsCommands`, `submitPresentCommands` y en los uploads de `GETexture`.
- **`loadMutex`**: protege `isLoadingState`/`loadProgress`/`loadMessage`/`loadError`.
- Regla: llamar a `scene->setLoading(true)` ANTES de lanzar el hilo de carga.

## Convenciones
- Prefijo de clases `GE`; comentarios Doxygen en español: bloque `// FUNCIÓN: nombre` + `// PROPÓSITO: descripción`.
- Línea en blanco entre sentencias; indentación con tabuladores.
- Inicialización Vulkan con `Vk*CreateInfo{}` y comprobación `== VK_SUCCESS`; `GEGraphicsContext` lanza `std::runtime_error` con mensaje y código.
- Structs GPU con `alignas` (ver `GEVertex.h`, `GETransform.h`, `GEMaterial.h`, `GELight.h`, `GEParticle.h`).
- `enum PipelineType`: `SKYBOX_PIPELINE=0, SCENE_PIPELINE=1, PARTICLE_PIPELINE=2`.

## Verificación tras cambios
1. `cmake --build --preset linux-debug` sin errores.
2. Ejecutar y comprobar: pantalla de carga visible (mín. 1.5 s), transición a escena, sin errores en el log.
3. Con capturas: verificar que los píxeles difieren entre frames (escena viva) y que aparecen los colores esperados (fuego/agua/rosa...).

## Documentación detallada
Para el mapa profundo por módulo, métodos y dependencias de cada clase: **`ProjectTFG/CODESTRUCTURE.md`**.

## graphify

This project has a knowledge graph at graphify-out/ with god nodes, community structure, and cross-file relationships.

When the user types `/graphify`, use the installed graphify skill or instructions before doing anything else.

Rules:
- For codebase questions, first run `graphify query "<question>"` when graphify-out/graph.json exists. Use `graphify path "<A>" "<B>"` for relationships and `graphify explain "<concept>"` for focused concepts. These return a scoped subgraph, usually much smaller than GRAPH_REPORT.md or raw grep output.
- Dirty graphify-out/ files are expected after hooks or incremental updates; dirty graph files are not a reason to skip graphify. Only skip graphify if the task is about stale or incorrect graph output, or the user explicitly says not to use it.
- If graphify-out/wiki/index.md exists, use it for broad navigation instead of raw source browsing.
- Read graphify-out/GRAPH_REPORT.md only for broad architecture review or when query/path/explain do not surface enough context.
- After modifying code, run `graphify update .` to keep the graph current (AST-only, no API cost).
