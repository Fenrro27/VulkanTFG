# ROADMAP — Camino a seguir

Este documento define la hoja de ruta del proyecto: cómo evolucionar el motor sin romper la coherencia con la memoria del TFG y cómo montar nuevas funciones encima del motor. Se implementa **poco a poco**, fase a fase.

## 1. Visión general

Dos repositorios con roles separados:

| Repo | Rol | Versionado |
|---|---|---|
| **VulkanTFG** (este repo) | Código de la memoria del TFG. **SOLO mejoras** sobre la base: rendimiento, bugs, build, documentación. **Ninguna función nueva.** | v1.x |
| **Fenrir** (fork futuro) | Funciones nuevas: refactor motor→librería, visor 3D, más apps. | v2.x |

**Regla de versionado:**
- `v1.x`: mejoras directas de la base del TFG.
- `v2.x`: funciones nuevas (vive en el fork Fenrir).
- Cada release debe incluir: **memoria** (fuente LaTeX + PDF) y **ejecutables** para **Windows, Linux y macOS**.

**Regla del repo:** el nombre `VulkanTFG` **no se renombra**: la memoria lo referencia explícitamente en `Memoria/Secciones/Anexo.tex:10` y en `Memoria/bibliografia.bib:169-172`.

## 2. Estado actual

- `v1.0` (tag): entrega de la defensa del TFG (`main` en `a7cecfc`).
- `develop` en `4e940e2`: reorganización en subcarpetas (Core/Render/Resources/Geometry/Objects/Particles/Types), documentación (AGENTS.md, CODESTRUCTURE.md, README), Graphify y barra de carga suave. **Aún sin release.**
- Mejoras de la memoria: todas implementadas (ver Fase 3).

## 3. Fase — Mejoras de la memoria: checklist de verificación

Las mejoras descritas en el capítulo *Mejoras respecto al código inicial* de la memoria ya están en el código:

- [x] Punteros inteligentes (`std::unique_ptr` / `std::shared_ptr`) en GEObject, GEPiece, GEScene, GEParticlesSystem, GEApplication.
- [x] Capa de validación de Vulkan activada (`GEGraphicsContext.cpp:275`).
- [x] TinyObjLoader con deduplicación de vértices (`GEModel.cpp`, `std::unordered_map`).
- [x] Modos de cámara Libre / FPS / Orbital (`GECamera`).
- [x] Interfaz ImGui (telemetría y controles).
- [x] Gestión de dependencias con vcpkg.

Pendiente de verificación funcional (cada cambio se comprueba con build + run):

- [ ] Build limpio: `cmake --build --preset linux-debug` (y release).
- [ ] Ejecutar y comprobar: pantalla de carga (mín. 1.5 s), transición a escena, sin errores en el log y sin mensajes de la capa de validación.
- [ ] Probar los tres modos de cámara (Libre/FPS/Orbital) y sus atajos.
- [ ] Comprobar carga de los modelos OBJ (fontain, campfire, newFountain, train) y sus texturas.
- [ ] Comprobar los tres sistemas de partículas (fuego, humo, agua) y su FPS estable.

## 4. Fase — macOS: arreglar la versión (prerequisito de las releases multi-SO)

El README marca macOS como *"Aún no compatible"*. Problemas detectados y tareas:

- [ ] `vcpkg.json`: las features `xcb`/`xlib`/`wayland` de `vulkan-loader` son solo Linux → condicionarlas por plataforma y añadir **`vulkan-moltenvk`** para macOS.
- [ ] Instancia Vulkan: solicitar **`VK_KHR_portability_enumeration`** y activar el flag **`VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR`** (requerido por MoltenVK) en `GEGraphicsContext`.
- [ ] Device: añadir **`VK_KHR_portability_subset`** a las extensiones de dispositivo cuando sea necesario (MoltenVK).
- [ ] Compilación: verificar que `glslc`/`glslangValidator` del SDK de macOS compila los shaders y que el `POST_BUILD` despliega `shaders/`, `textures/` y `models/` junto al binario (el fallback por filesystem ya existe en no-Windows).
- [ ] Build y prueba **solo en CI** (runner `macos-latest`); no hay Mac local.

## 5. Fase — CI multi-SO: ejecutables + memoria por sistema

No existe `.github/` hoy. Tareas:

- [ ] Añadir GitHub Actions con matrix de SO: `windows-latest`, `ubuntu-latest`, `macos-latest`.
- [ ] Configurar vcpkg (`VCPKG_ROOT`) y presets `*-release` en cada runner.
- [ ] Build y **subida de artefactos**: binario + `shaders/` + `textures/` + `models/` (+ memoria PDF).
- [ ] Publicar los artefactos y la memoria automáticamente en cada release (workflow disparado por tag `v*`).

## 6. Fase — Memoria: actualizaciones

- [ ] Añadir un capítulo/sección con el trabajo posterior a la defensa: reestructuración en módulos, Graphify, pantalla de carga con progreso y barra suave.
- [ ] Actualizar el Anexo (enlaces) y la Conclusión si procede.
- [ ] **Mantener** el enlace `https://github.com/Fenrro27/VulkanTFG` (no renombrar el repo).
- [ ] Regenerar el PDF (`doxygen`/`pdflatex`).

## 7. Fase — Releases de la serie v1.x

- [ ] **`v1.1`**: espera a que macOS esté arreglado (Fase 4). Primera release multi-SO: merge `develop` → `main`, tag `v1.1`, release con notas + **memoria + 3 ejecutables**.
- [ ] `v1.2`, `v1.3`, ...: futuras mejoras de base (rendimiento, bugs, build, docs). Cada una con memoria + ejecutables.

## 8. Fase — Fork Fenrir: funciones nuevas (v2.x)

**Se ejecuta solo al completar las Fases 3–7.**

- [ ] Crear el fork: `gh repo fork Fenrro27/VulkanTFG --fork-name Fenrir --clone` (conserva el enlace `upstream` para sincronizar mejoras v1.x).
- [ ] **v2.0 — Refactor**: extraer el motor a una librería estática `GE` + estructura multi-target: `apps/demo` (el TFG actual) y `apps/viewer`; funciones CMake compartidas para shaders y recursos.
- [ ] **v2.1+ — Visor 3D**: carga de modelos OBJ, cámara órbita + grid, panel ImGui de materiales y luces; opcional glTF y drag & drop.
- [ ] Releases v2.x reutilizando el CI (memoria + ejecutables por SO).

## 9. Orden de trabajo recomendado

1. Verificar las mejoras de la memoria (Fase 3).
2. Arreglar macOS (Fase 4).
3. Montar el CI multi-SO (Fase 5).
4. Actualizar la memoria (Fase 6).
5. Publicar `v1.1` con memoria + 3 ejecutables (Fase 7).
6. Crear el fork Fenrir y empezar v2.x (Fase 8).
