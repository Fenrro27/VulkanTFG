cmake_minimum_required(VERSION 3.15)
project(ProjectTFG LANGUAGES CXX)

# 1. Buscar herramientas necesarias
find_program(GLSL_VALIDATOR glslangValidator)
if(NOT GLSL_VALIDATOR)
    message(WARNING "No se encontró glslangValidator. Los shaders no se compilarán.")
endif()

# 2. Lista de Shaders
file(GLOB SHADER_SOURCES "shaders/*.vert" "shaders/*.frag" "shaders/*.comp")

# 3. Lógica de compilación de Shaders (Ruta simple para evitar errores)
set(SPV_OUTPUTS "")
if(GLSL_VALIDATOR)
    foreach(SHADER ${SHADER_SOURCES})
        get_filename_component(FILE_NAME ${SHADER} NAME_WE)
        get_filename_component(FILE_EXT ${SHADER} EXT)
        string(SUBSTRING ${FILE_EXT} 1 -1 SUFFIX)
        
        # Generamos el .spv en una carpeta temporal de la build
        set(OUTPUT_NAME "${CMAKE_CURRENT_BINARY_DIR}/compiled_shaders/${FILE_NAME}_${SUFFIX}.spv")
        
        add_custom_command(
            OUTPUT ${OUTPUT_NAME}
            COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/compiled_shaders"
            COMMAND ${GLSL_VALIDATOR} -V ${SHADER} -o ${OUTPUT_NAME}
            DEPENDS ${SHADER}
            COMMENT "Compilando shader: ${SHADER}"
        )
        list(APPEND SPV_OUTPUTS ${OUTPUT_NAME})
    endforeach()
endif()

# 4. Lista de archivos de código
file(GLOB SRC_FILES "*.cpp" "*.h")

# 5. Crear el Ejecutable (Ahora el objetivo "Project6" ya existe)
add_executable(${PROJECT_NAME} ${SRC_FILES} ${SPV_OUTPUTS})

# 6. Dependencias con vcpkg
find_package(Vulkan REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(tinyobjloader CONFIG REQUIRED)
find_path(STB_INCLUDE_DIR "stb_image.h")

target_link_libraries(${PROJECT_NAME} PRIVATE 
    Vulkan::Vulkan 
    glfw 
    glm::glm 
    imgui::imgui
    tinyobjloader::tinyobjloader
)

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR} 
    ${STB_INCLUDE_DIR}
)

# 7. POST-BUILD: Copiar TODO a la carpeta del ejecutable
# Ahora sí podemos usar $<TARGET_FILE_DIR> porque el target ya existe
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders"
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/textures"
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/models"
    
    # Copiar shaders compilados
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_BINARY_DIR}/compiled_shaders" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders"
        
    # Copiar recursos originales
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/textures" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/textures"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/models" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/models"
        
    COMMENT "Desplegando shaders y recursos al directorio del ejecutable..."
)
