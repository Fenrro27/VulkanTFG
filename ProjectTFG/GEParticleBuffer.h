/**
 * @file GEParticleBuffer.h
 * @brief Archivo GEParticleBuffer.h
 */
#pragma once

#include <vulkan/vulkan.h>
#include "GEGraphicsContext.h"

	//
	// CLASE: GEParticleBuffer
	//
	// DESCRIPCIÓN: Clase que describe un buffer de particulas
	//
/**
 * @class GEParticleBuffer
 * @brief Class GEParticleBuffer
 */
class GEParticleBuffer
{
public:
	VkBuffer buffer;
	VkDeviceMemory memory;

	GEParticleBuffer(GEGraphicsContext* gc, size_t size, const void* data);
	/**
	 * @brief Función destroy
	 */
	void destroy(GEGraphicsContext* gc);
};
