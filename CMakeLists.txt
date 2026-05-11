cmake_minimum_required(VERSION 3.20) # Versión moderna para mejor soporte de Vulkan

project(VulkanTFG CXX)

# Coherencia con el estándar de C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

################################################################################
# Configuración de vcpkg (debe ir ANTES de find_package)
################################################################################
# Si no quieres pasar el -DCMAKE_TOOLCHAIN_FILE por consola, 
# puedes definirlo aquí si conoces la ruta, pero lo ideal es por terminal.

################################################################################
# Búsqueda de Dependencias vía vcpkg / Sistema
################################################################################
find_package(Vulkan REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

# Nota: Para ImGui en vcpkg, necesitas instalar los bindings (vulkan y glfw)
# vcpkg install imgui[vulkan-binding,glfw-binding]

################################################################################
# Configuración del Ejecutable
################################################################################
# Añadimos los subproyectos (donde están tus archivos .cpp)
add_subdirectory(Project6)
