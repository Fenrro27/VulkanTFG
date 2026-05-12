cmake_minimum_required(VERSION 3.20)

project(VulkanTFG CXX)

# Coherencia con el estándar de C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

################################################################################
# Configuración del Ejecutable
################################################################################
# Añadimos los subproyectos (donde están tus archivos .cpp)
add_subdirectory(ProjectTFG)

# --- FUNCIÓN PARA EJECUTAR DESDE TERMINAL ---
# Esto permite hacer: cmake --build --preset win-debug --target run
add_custom_target(run
    COMMAND $<TARGET_FILE:ProjectTFG>
    DEPENDS ProjectTFG
    WORKING_DIRECTORY $<TARGET_FILE_DIR:ProjectTFG>
    COMMENT "Lanzando el motor Vulkan..."
)
