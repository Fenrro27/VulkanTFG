/**
 * @file GEAssets.h
 * @brief Definición de las utilidades de acceso a los recursos del motor (embebidos o en disco).
 */
#pragma once

#include <cstddef>
#include <string>
#include <vector>

/**
 * @struct EmbeddedResource
 * @brief Descriptor de un recurso incrustado dentro del ejecutable.
 *
 * Esta estructura es rellenada por el fichero generado automáticamente por CMake
 * (embedded_resources.cpp) cuando la opción GE_EMBED_RESOURCES está activa.
 */
struct EmbeddedResource
{
	const char* key;         /**< Clave lógica del recurso (p. ej. "textures/wood.jpg"). */
	const unsigned char* data; /**< Puntero a los bytes del recurso. */
	size_t size;             /**< Tamaño en bytes del recurso. */
};

/**
 * @class GEAssets
 * @brief Utilidades de acceso a los recursos del motor.
 *
 * Cuando los recursos están incrustados (GE_EMBED_RESOURCES=ON) se devuelven desde
 * los arrays generados por CMake; en caso contrario se leen desde disco usando la
 * misma clave como ruta relativa.
 */
namespace GEAssets
{
	/**
	 * @brief Devuelve el contenido de un recurso como un buffer de bytes.
	 * @param key Clave lógica del recurso (p. ej. "shaders/scene_shader_vert.spv").
	 * @return Vector con el contenido binario del recurso.
	 */
	std::vector<char> getFileVector(const std::string& key);

	/**
	 * @brief Devuelve los bytes de un shader a partir de su ID de recurso.
	 * @param resource ID del recurso (IDR_VERT_SCENE, IDR_COMPUTE_HUMO, ...).
	 * @return Vector con el SPIR-V compilado del shader.
	 */
	std::vector<char> getShaderBytes(int resource);
}
