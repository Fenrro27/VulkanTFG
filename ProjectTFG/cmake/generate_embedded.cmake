# generate_embedded.cmake
#
# Genera embedded_resources.cpp a partir de un manifiesto de recursos.
# Cada línea del manifiesto tiene el formato:  <clave_logica>\t<ruta_absoluta>
#
# Se invoca desde CMakeLists.txt como un custom command en build time, con las
# variables GE_MANIFEST_FILE (entrada) y GE_OUTPUT_CPP (salida).

if(NOT DEFINED GE_MANIFEST_FILE)
    message(FATAL_ERROR "generate_embedded.cmake: falta GE_MANIFEST_FILE")
endif()
if(NOT DEFINED GE_OUTPUT_CPP)
    message(FATAL_ERROR "generate_embedded.cmake: falta GE_OUTPUT_CPP")
endif()

file(READ "${GE_MANIFEST_FILE}" _manifest)
string(REGEX MATCHALL "[^\r\n]+" _lines "${_manifest}")

file(WRITE "${GE_OUTPUT_CPP}" "#include \"GEAssets.h\"\n")
file(APPEND "${GE_OUTPUT_CPP}" "// Fichero generado automáticamente por CMake. No editar.\n")

set(_idx 0)
set(_table "")

foreach(_line IN LISTS _lines)
    if(_line STREQUAL "")
        continue()
    endif()

    string(REPLACE "\t" ";" _kv "${_line}")
    list(GET _kv 0 _key)
    list(GET _kv 1 _path)

    if(NOT EXISTS "${_path}")
        message(FATAL_ERROR "generate_embedded.cmake: recurso no encontrado: ${_path}")
    endif()

    file(READ "${_path}" _hex HEX)
    string(REGEX REPLACE "([0-9a-fA-F][0-9a-fA-F])" "0x\\1, " _bytes "${_hex}")

    file(APPEND "${GE_OUTPUT_CPP}" "static const unsigned char kR${_idx}[] = { ${_bytes} };\n")
    string(APPEND _table "  { \"${_key}\", kR${_idx}, sizeof(kR${_idx}) },\n")

    math(EXPR _idx "${_idx} + 1")
endforeach()

file(APPEND "${GE_OUTPUT_CPP}" "\n// Símbolos con enlace externo (definidos aquí, declarados en GEAssets.cpp)\n")
file(APPEND "${GE_OUTPUT_CPP}" "extern const EmbeddedResource kEmbeddedResources[] = {\n${_table}};\n")
file(APPEND "${GE_OUTPUT_CPP}" "extern const size_t kEmbeddedResourcesCount = ${_idx};\n")