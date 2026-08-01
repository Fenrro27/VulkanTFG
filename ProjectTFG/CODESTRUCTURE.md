# CODESTRUCTURE.md — Mapa profundo del motor (ProjectTFG)

Referencia a demanda. Complementa a `AGENTS.md` (resumen por sesión). Para cada clase: archivo, rol, API pública y dependencias. Nombres `archivo:línea` orientativos.

> Estructura en subcarpetas desde la reorganización: `Core/` (app, escena, cámara, skybox), `Render/` (Vulkan), `Resources/` (recursos GPU), `Geometry/` (figuras), `Objects/` (objetos/modelos), `Particles/`, `Types/` (structs GPU). Todos los includes son planos (`#include "GEGraphicsContext.h"`) y se resuelven por las rutas de include del CMake.

Índice:
1. [Núcleo gráfico](#1-núcleo-gráfico)
2. [Recursos GPU](#2-recursos-gpu)
3. [Figuras paramétricas](#3-figuras-paramétricas)
4. [Objetos y modelos](#4-objetos-y-modelos)
5. [Cámara](#5-cámara)
6. [Partículas y compute](#6-partículas-y-compute)
7. [Escena, skybox y aplicación](#7-escena-skybox-y-aplicación)
8. [Shaders](#8-shaders)
9. [Structs GPU](#9-structs-gpu)

---

## 1. Núcleo gráfico (`Render/`)

### GEGraphicsContext — `Render/GEGraphicsContext.h/.cpp` (1430 líneas)
Instancia Vulkan, surface, device físico/lógico y familias de colas.
- Miembros públicos: `instance`, `surface`, `physicalDevice`, `device`, `graphicsQueueFamilyIndex`, `presentQueueFamilyIndex`, `graphicsQueueMutex`.
- API: `GEGraphicsContext(GLFWwindow*)`, `findMemoryType()`, `findDepthFormat()`.
- Init privado: `createInstance` → `createSurface` → `pickPhysicalDevice` (`isDeviceSuitable`, `showDevices`) → `createLogicalDevice`.
- Errores: `std::runtime_error` con mensaje + código de retorno de Vulkan.
- `graphicsQueueMutex` serializa el acceso a la cola de gráficos (ver §7, modelo de hilos).

### GEDrawingContext — `Render/GEDrawingContext.h/.cpp` (982 líneas)
Swapchain, image views, colas, sincronización (semáforos/fences), `MAX_FRAMES_IN_FLIGHT = 2`.
- API: `recreate`, `getFormat`, `getExtent`, `getImageCount`, `getCurrentImage`, `waitForNextImage`, `submitGraphicsCommands`, `submitPresentCommands`, `waitIdle`.
- `submitGraphicsCommands`/`submitPresentCommands`: toman `std::lock_guard<std::mutex>` sobre `gc->graphicsQueueMutex`.
- Miembro público: `imageViews`.

### GECommandContext — `Render/GECommandContext.h/.cpp` (146 líneas)
Command pool + un command buffer por imagen de swapchain.
- API: `beginCommandBuffers()`, `endCommandBuffers()`; miembro público `commandBuffers`.

### GERenderingContext — `Render/GERenderingContext.h/.cpp` (733 líneas)
Render pass, pipelines gráficos, descriptor set layouts, pipeline layouts, depth buffers y framebuffers.
- `PipelineType` en `Core/GEScene.h:30`: `SKYBOX_PIPELINE=0`, `SCENE_PIPELINE=1`, `PARTICLE_PIPELINE=2`.
- API: `addGraphicsPipeline(GEPipelineConfig*)`, `setActivePipeline`, `getActiveDescriptorSetLayout`, `getActivePipelineLayout`, `insertBeginCommands`, `insertPipelineCommands`, `insertEndCommands`, `getRenderPass`, `getImageCount`.
- Interno: `createRenderPass`, `createGraphicsPipeline` (todas las `createPipeline*StateCreateInfo`), `createDepthBuffers`, `createFramebuffers`, `createShaderModule` (SPIR-V desde recurso), `getFileFromResource`.

### GEDepthBuffer — `Render/GEDepthBuffer.h/.cpp`
Buffer de profundidad (imagen + view + memoria) por imagen de swapchain. Creado por `GERenderingContext::createDepthBuffers`.

---

## 2. Recursos GPU (`Resources/`)

### GETexture — `Resources/GETexture.h/.cpp` (1102 líneas)
Textura 2D o cubemap: staging buffer → imagen → image view → sampler.
- Constructores: `GETexture(gc, const char* filename)` (2D) y `GETexture(gc, const char** filename)` (cubemap, 6 caras).
- Miembros: `textureImage`, `textureImageView`, `textureSampler`, `textureWidth/Height`.
- Uploads con `graphicsQueueMutex` (compatible con `loaderThread`).
- `destroy(gc)` libera imagen/memoria/view/sampler.

### GEVertexBuffer / GEIndexBuffer / GEUniformBuffer — `Resources/GEVertexBuffer.h` / `Resources/GEIndexBuffer.h` / `Resources/GEUniformBuffer.h`
- VBO/IBO: creados en `initialize()` de la figura/objeto; `addCommands` bindea buffer + offsets.
- UBO: `buffers[i]` por frame (una copia por imagen de swapchain).

### GEDescriptorSet — `Resources/GEDescriptorSet.h/.cpp`
Descriptor pool + sets por frame; `descriptorSets[index]` expone cada set.

### GEPipelineConfig — `Resources/GEPipelineConfig.h`
Configuración declarativa de pipeline (no toca Vulkan directamente).
- Campos: `vertex_shader`, `fragment_shader` (IDs de recurso SPIR-V), `attrStride`, `attrFormats`, `attrOffsets`, `descriptorTypes`, `descriptorStages`, `depthTestEnable`, `cullMode`, `extent`, `topology`, `depthWrite`.
- Las configuraciones concretas viven en `GEScene`: `createSkyboxPipelineConfig`, `createScenePipelineConfig`, `createParticlePipelineConfig` (`Core/GEScene.h:187-189`).

---

## 3. Figuras paramétricas (`Geometry/`)

### GEFigure (base) — `Geometry/GEFigure.h/.cpp` (227 líneas)
Malla genérica: vértices + índices + transformación + material + luz + textura.
- API: `initialize`, `destroy`, `addCommands`, `update`, `resetLocation`, `setLocation`, `translate`, `rotate`, `setMaterial`, `setLight`, `setTexture`, `scale`.
- Interno: `vbo`, `ibo`, `transformBuffer`, `materialBuffer`, `lightBuffer`, `dset`.
- Subclases (constructores paramétricos):
  - `GECube(float s)` — `Geometry/GECube.h:24`
  - `GESphere(int p, int m, float r)` — `Geometry/GESphere.h:21`
  - `GECone(GLint p, GLint m, GLfloat h, GLfloat r)` — `Geometry/GECone.h:21`
  - `GECylinder(int p, int m, float r, float l)` — `Geometry/GECylinder.h:21`
  - `GEPyramid(float s)` — `Geometry/GEPyramid.h:24`
  - `GEDisk(int p, int m, float r0, float r1)` — `Geometry/GEDisk.h:24`
  - `GEIcosahedron(float r)` — `Geometry/GEIcosahedron.h:24`
  - `GETorus(int p, int m, float r0, float r1)` — `Geometry/GETorus.h` / `Geometry/GETorus.cpp` (patrón de referencia: bucles `i<=m`, `j<=p`, `vertices` e `indices` rellenados con trigonometría).

### GEGround — `Geometry/GEGround.h/.cpp`
Suelo (subclase de `GEFigure`). Malla cuadriculada grande (p. ej. 150×150 en la escena).

---

## 4. Objetos y modelos (`Objects/`)

### GEObject (base) — `Objects/GEObject.h/.cpp` (286 líneas)
Contenedor de piezas (`GEPiece`) con modelo compuesto.
- API: `destroy`, `addCommands`, `update`, `resetLocation`, `setLocation`, `translate`, `rotate`, `setLight`, `setTexture`.
- Miembros: `materials`, `textures` (shared_ptr), `pieces` (unique_ptr), `model`.

### GEPiece — `Objects/GEPiece.h/.cpp` (214 líneas)
Pieza individual (malla + materiales). API similar a `GEFigure`; `update` recibe `model` de su objeto padre. `setVertices`/`setIndices` permiten construir piezas en código.

### GEModel — `Objects/GEModel.h/.cpp` (282 líneas)
Carga `.obj` con TinyObjLoader → piezas (`GEPiece`), materiales y texturas embebidas del MTL.
- Constructor: `GEModel(gc, const std::string& path, float scale = 1.0f)`.
- `initialize(gc, rc)` prepara los buffers.

---

## 5. Cámara (`Core/`)

### GECamera — `Core/GECamera.h/.cpp` (1452 líneas)
Cámara con modos `FREE` / `FPS` / `OBSERVING`.
- `struct ObservationPoint { glm::mat4 location; std::string name; }` (`Core/GECamera.h:15`).
- API: `getViewMatrix`, `setPosition`, `setDirection`, `update`, `processMouse`, `setNextMode`, `addObservationPoint`, `nextObservationPoint`, `prevObservationPoint`, `getCurrentObservationName`, `setObservationDistance`, `stopAllMovement`.
- Control: flags `setMoveFront/Back/Left/Right/Up/Down`, `setTurnLeft/Right/Up/Down/CW/CCW`; internamente `moveFront()`/`turnRight()`... y `updateCameraVectorsFromEuler` (yaw/pitch).
- En `OBSERVING`, orbita alrededor del punto activo a distancia `observationDistance`.

---

## 6. Partículas y compute (`Particles/`)

### GEParticlesSystem — `Particles/GEParticlesSystem.h/.cpp` (520 líneas)
Sistema de partículas con doble buffer GPU.
- Miembros: `particles`, `indices`, `location`, `material`, `light`, `pboA`/`pboB` (`GEParticleBuffer`), `ibo`, `transformBuffer`, `materialBuffer`, `lightBuffer`, `dset`, `emitterParams` + `emitterParamsBuffer`, `lastTime`.
- API: `initialize`, `destroy`, `addCommands`, `update`, `translate`, `rotate`, `setMaterial`, `setLight`, `getLocation`, `getParticlesBufferA/B`, `getParticlesSize`, `getParticlesCount`, `addParticle`, `getDescriptorSet`, `getEmitterParamsBuffer`, `updatePhysics`.
- **Importante**: `pboA` y `pboB` deben inicializarse con los mismos datos (ping-pong), si no el primer frame lee basura y las partículas quedan muertas.

### GEHumo / GEFuego / GEAgua — `Particles/GEHumo.h/.cpp`, `Particles/GEFuego.h/.cpp`, `Particles/GEAgua.h/.cpp`
Subclases de `GEParticlesSystem`; el constructor rellena `particles` con el emisor específico (humo, fuego, agua) usando `addParticle`.

### GEComputeShader — `Particles/GEComputeShader.h/.cpp` (273 líneas)
Pipeline compute (SPIR-V) + dispatch por sistema de partículas.
- Constructor: `GEComputeShader(gc, int shaderResource, uint32_t imageCount)`.
- API: `addParticleSystem(gc, imageCount, ps)`, `recordCommands(cb, systemIndex, imageIndex, groupCountX)`, `destroy`.
- `recordCommands` enlaza `pboA`→entrada, `pboB`→salida (alterna según imagen), `emitterParamsBuffer` y despacha en grupos de 256.
- Recursos de shader: `IDR_COMPUTE_HUMO`, `IDR_COMPUTE_FUEGO`, `IDR_COMPUTE_AGUA` (en `resource.h`).

### GEParticleBuffer — `Particles/GEParticleBuffer.h`
Buffer de vértices de partículas (VBO GPU) con sus `VkBuffer`/`VkDeviceMemory` por frame.

### GEEmitterParams — `Particles/GEEmitterParams.h`
Struct `alignas(16)`: `emitterPos` (vec4) y `force` (vec4). Uniform del compute.

### GEParticle — `Particles/GEParticle.h`
`typedef struct alignas(16)`: `color`, posición/velocidad, tamaño, `activeTTL`, etc. Formato consumido por el compute shader.

---

## 7. Escena, skybox y aplicación (`Core/`)

### GEScene — `Core/GEScene.h/.cpp` (1365 líneas)
Contiene la escena completa y la carga en segundo plano.
- Miembros: `rc` (`GERenderingContext`), `skybox`, `camera`, `computeShaders`, `figures`, `objects`, `particleSystem`, `textures`, `projection`, `particleCount = 10000`, estado de carga (`isLoadingState`, `loadProgress`, `loadMessage`, `loadError`) protegido por `loadMutex`.
- Constructor ligero: crea render pass, pipelines y cámara. **La carga pesada está en `loadAssets(gc, dc, cc)`** (hilo separado): skybox → texturas → suelo → figuras → objetos → compute shaders → sistemas de partículas → `setLoadProgress(1.0f)` + `setLoading(false)`.
- API de carga: `setLoading`, `isLoading`, `setLoadProgress`, `getLoadProgress`, `getLoadMessage`, `setLoadError`, `hasLoadError`, `getLoadError`.
- API de render: `update` (cámara + partículas), `updatePhysics` (paso fijo), `recordComputeCommands(cb, i, queryPool, physicsSteps)` (timestamps + dispatch compute + barreras), `drawGraphicsObjects(cb, i)` (skybox → figuras → objetos → partículas), `getTotalParticleCount`.
- Entrada: `key_action`, `mouse_action`, `mouse_button_action`, `aspect_ratio`, `toggle_camera_mode`.
- `recreate(gc, dc, cc)` para re-inicializar tras resize.
- Pipelines: `createSkyboxPipelineConfig`, `createScenePipelineConfig`, `createParticlePipelineConfig`.

### GESkybox — `Core/GESkybox.h/.cpp` (121 líneas)
Cubo con cubemap; se dibuja primero con el pipeline de skybox.

### GEApplication — `Core/GEApplication.h/.cpp` (874 líneas)
Ventana GLFW (800×600), bucle principal, ImGui y pantalla de carga.
- Miembros: `window` (unique_ptr + `GLFWWindowDeleter`), `gc`, `dc`, `cc`, `scene`, `imguiPool`, `queryPool` (timestamps de compute), `loaderThread`, `pendingResize`, `bigTitleFont`.
- Flujo: `run()` → `initWindow` → `GEGraphicsContext` → `GEDrawingContext` → `GECommandContext` → `GERenderingContext` → `GEScene` → `scene->setLoading(true)` + `loaderThread` → `loadingLoop()` → `mainLoop()`.
- `mainLoop`: paso fijo de física (aprox. 60 Hz) + interpolación `alpha` → `draw(deltaTime, physicsSteps, alpha, frameTime)` → `recordComputeCommands` + `drawGraphicsObjects` → submit → present.
- `loadingLoop`/`drawLoading`/`renderLoadingScreen`: pantalla de carga animada (spinner, barra con %, título "VULKAN TFG" con `bigTitleFont` a 72 px), mínimo 1.5 s; si `hasLoadError()` muestra el error.
- ImGui: `inicializarImGui`, `ControlsGUI` (top bar con FPS, modo de cámara, `Particulas GPU` ms desde `queryPool`, `nParticulas`).
- Callbacks estáticos: `keyCallback`, `mouseButtonCallback`, `cursorPositionCallback`, `framebufferResizeCallback` (fija `pendingResize`).

### main.cpp
`main()` (raíz de `ProjectTFG/`): crea `GEApplication`, `app.run()` dentro de try/catch (`std::cerr` + `EXIT_FAILURE`).

---

## 8. Shaders
Compilados a SPIR-V y embebidos (`.rc`). Fuente GLSL en `ProjectTFG/shaders/` (`CompileShaders.bat`).

| Shader | Pipeline | Uso |
|---|---|---|
| `skybox_shader.vert/frag` | SKYBOX_PIPELINE | Cubo de cielo |
| `scene_shader.vert/frag` | SCENE_PIPELINE | Figuras, suelo, modelos (Phong) |
| `particles_shader.vert/frag` | PARTICLE_PIPELINE | Dibujo de partículas (point list) |
| `smoke.comp` | compute | Simulación humo (IDR_COMPUTE_HUMO) |
| `fire.comp` | compute | Simulación fuego (IDR_COMPUTE_FUEGO) |
| `water.comp` | compute | Simulación agua (IDR_COMPUTE_AGUA) |

Convención de los compute: leen el PBO de entrada (`pboA`/`pboB` alternado), preservan/renuevan `activeTTL`, aplican `emitterPos`/`force` y escriben el PBO de salida.

---

## 9. Structs GPU (`Types/`)
Con `alignas` para coincidir con los shaders:
- `GEVertex` (`Types/GEVertex.h:18`): `pos` (16), `norm` (16), `tex` (8) + `operator==`.
- `GETransform` (`Types/GETransform.h`): `MVP`.
- `GEMaterial` (`Types/GEMaterial.h`): `Ka`, `Kd`, `Ks`, `Shininess`.
- `GELight` (`Types/GELight.h`): `Ldir`, intensidades/color.
- `GEParticle` (`Particles/GEParticle.h`), `GEEmitterParams` (`Particles/GEEmitterParams.h`): ver §6.
- `GESkyboxVertex` (`Core/GESkyboxVertex.h`), `GESkyboxTransform` (`Core/GESkyboxTransform.h`).
