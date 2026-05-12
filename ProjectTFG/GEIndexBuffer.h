/**
 * @file GEIndexBuffer.h
 * @brief Archivo GEIndexBuffer.h
 */
#pragma once

#include <vulkan/vulkan.h>
#include "GEGraphicsContext.h"

//
// CLASE: GEIndexBuffer
//
// DESCRIPCIÓN: Clase que describe un buffer de índices
//
/**
 * @class GEIndexBuffer
 * @brief Class GEIndexBuffer
 */
class GEIndexBuffer
{
public:
	VkBuffer buffer;
	VkDeviceMemory memory;

	GEIndexBuffer(GEGraphicsContext* gc, size_t size, const void* data);
	/**
	 * @brief Función destroy
	 */
	void destroy(GEGraphicsContext* gc);
};

