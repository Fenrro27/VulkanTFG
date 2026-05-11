set(PROJECT_NAME Project6)

# 1. Buscar el compilador de shaders (glslangValidator)
find_program(GLSL_VALIDATOR NAMES glslangValidator)
if(NOT GLSL_VALIDATOR)
    message(FATAL_ERROR "No se encontró glslangValidator. Instálalo con: sudo apt install vulkan-tools")
endif()

# 2. Lista de Shaders (Archivos fuente)
set(SHADER_SOURCES
    "shaders/scene_shader.vert"
    "shaders/scene_shader.frag"
    "shaders/skybox_shader.vert"
    "shaders/skybox_shader.frag"
    "shaders/particles_shader.vert"
    "shaders/particles_shader.frag"
    "shaders/fire.comp"
    "shaders/smoke.comp"
    "shaders/water.comp"
)

# 3. Lógica de compilación automática de Shaders
set(SPV_OUTPUTS "")
foreach(SHADER ${SHADER_SOURCES})
    get_filename_component(FILE_NAME ${SHADER} NAME_WE) # nombre sin extensión
    get_filename_component(FILE_EXT ${SHADER} EXT)     # .vert, .frag, .comp
    
    # Determinamos el nombre de salida (ej: scene_vert.spv)
    string(SUBSTRING ${FILE_EXT} 1 4 SUFFIX) # vert, frag o comp
    set(OUTPUT_NAME "${CMAKE_CURRENT_BINARY_DIR}/shaders/${FILE_NAME}_${SUFFIX}.spv")
    
    # Comando de compilación
    add_custom_command(
        OUTPUT ${OUTPUT_NAME}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/shaders"
        COMMAND ${GLSL_VALIDATOR} -V ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER} -o ${OUTPUT_NAME}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${SHADER}
        COMMENT "Compilando shader: ${SHADER}"
    )
    list(APPEND SPV_OUTPUTS ${OUTPUT_NAME})
endforeach()

# 4. Lista de archivos de código
set(SRC_FILES
    "main.cpp" "GEApplication.cpp" "GECamera.cpp" "GEScene.cpp" "GESkybox.cpp"
    "GEDepthBuffer.cpp" "GEDescriptorSet.cpp" "GEIndexBuffer.cpp" "GETexture.cpp"
    "GEUniformBuffer.cpp" "GEVertexBuffer.cpp" "GEAgua.cpp" "GEComputeShader.cpp"
    "GEFuego.cpp" "GEHumo.cpp" "GEParticleBuffer.cpp" "GEParticlesSystem.cpp"
    "GECommandContext.cpp" "GEDrawingContext.cpp" "GEGraphicsContext.cpp"
    "GEPipelineConfig.cpp" "GERenderingContext.cpp" "GECone.cpp" "GECube.cpp"
    "GECylinder.cpp" "GEDisk.cpp" "GEFigure.cpp" "GEGround.cpp" "GEIcosahedron.cpp"
    "GEModel.cpp" "GEObject.cpp" "GEPiece.cpp" "GEPyramid.cpp" "GESphere.cpp" "GETorus.cpp"
)

# 5. Crear el Ejecutable
add_executable(${PROJECT_NAME} ${SRC_FILES} ${SPV_OUTPUTS})

# 6. Dependencias con vcpkg
find_package(Vulkan REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(Stb CONFIG REQUIRED)
find_package(tinyobjloader CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE 
    Vulkan::Vulkan 
    glfw 
    glm::glm 
    imgui::imgui
  #  stb::stb
    tinyobjloader::tinyobjloader
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${Stb_INCLUDE_DIR})

# 7. Copiar Texturas y Modelos a la carpeta build para que el .exe los encuentre
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/textures ${CMAKE_CURRENT_BINARY_DIR}/textures
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/models ${CMAKE_CURRENT_BINARY_DIR}/models
    COMMENT "Copiando recursos (modelos y texturas) al directorio de salida..."
)
