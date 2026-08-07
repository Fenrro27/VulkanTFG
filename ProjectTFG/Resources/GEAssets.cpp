/**
 * @file GEAssets.cpp
 * @brief Implementación de las utilidades de acceso a los recursos del motor.
 */

#include "GEAssets.h"

#include "resource.h"

#include <fstream>

#include <stdexcept>



// Tabla de recursos incrustados, definida en el fichero generado por CMake
// (embedded_resources.cpp). Contiene cero entradas cuando GE_EMBED_RESOURCES=OFF.
extern const EmbeddedResource kEmbeddedResources[];

extern const size_t kEmbeddedResourcesCount;



namespace GEAssets
{

	//
	// FUNCIÓN: GEAssets::getFileVector(const std::string& key)
	//
	// PROPÓSITO: Devuelve el contenido binario de un recurso. Si el recurso está
	// incrustado en el ejecutable se copia desde su array; si no, se lee de disco
	// usando la misma clave como ruta relativa.
	//
	std::vector<char> getFileVector(const std::string& key)
	{
		for (size_t i = 0; i < kEmbeddedResourcesCount; ++i)
		{
			if (key == kEmbeddedResources[i].key)
			{
				const unsigned char* data = kEmbeddedResources[i].data;

				if (kEmbeddedResources[i].size == 0)
				{
					return std::vector<char>();
				}

				return std::vector<char>(data, data + kEmbeddedResources[i].size);
			}
		}

		// Modo sin incrustar: se lee el archivo desde el disco
		std::ifstream file(key, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			throw std::runtime_error("recurso no encontrado: " + key);
		}

		size_t fileSize = (size_t)file.tellg();

		std::vector<char> buffer(fileSize);

		file.seekg(0);

		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}

	//
	// FUNCIÓN: GEAssets::getShaderBytes(int resource)
	//
	// PROPÓSITO: Traduce un ID de recurso (resource.h) a la clave lógica del SPIR-V
	// y devuelve su contenido binario.
	//
	std::vector<char> getShaderBytes(int resource)
	{
		std::string fileName;

		switch (resource)
		{
			case IDR_VERT_SCENE: fileName = "shaders/scene_shader_vert.spv"; break;

			case IDR_FRAG_SCENE: fileName = "shaders/scene_shader_frag.spv"; break;

			case IDR_COMPUTE_HUMO: fileName = "shaders/smoke_comp.spv"; break;

			case IDR_COMPUTE_FUEGO: fileName = "shaders/fire_comp.spv"; break;

			case IDR_COMPUTE_AGUA: fileName = "shaders/water_comp.spv"; break;

			case IDR_VERT_PARTICLES: fileName = "shaders/particles_shader_vert.spv"; break;

			case IDR_FRAG_PARTICLES: fileName = "shaders/particles_shader_frag.spv"; break;

			case IDR_VERT_SKYBOX: fileName = "shaders/skybox_shader_vert.spv"; break;

			case IDR_FRAG_SKYBOX: fileName = "shaders/skybox_shader_frag.spv"; break;

			default: throw std::runtime_error("ID de shader desconocido");
		}

		return getFileVector(fileName);
	}
}
