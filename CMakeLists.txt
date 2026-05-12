cmake_minimum_required(VERSION 3.20)

project(VulkanTFG CXX)

# Coherencia con el estándar de C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

################################################################################
# Configuración del Ejecutable
################################################################################
# Añadimos los subproyectos (donde están tus archivos .cpp)
add_subdirectory(Project6)

# --- FUNCIÓN PARA EJECUTAR DESDE TERMINAL ---
# Esto permite hacer: cmake --build --preset win-debug --target run
add_custom_target(run
    COMMAND $<TARGET_FILE:Project6>
    DEPENDS Project6
    WORKING_DIRECTORY $<TARGET_FILE_DIR:Project6>
    COMMENT "Lanzando el motor Vulkan..."
)
