/**
 * @file GEVertexBuffer.h
 * @brief Archivo GEVertexBuffer.h
 */
#pragma once

#include <vulkan/vulkan.h>
#include "GEGraphicsContext.h"

//
// CLASE: GEVertexBuffer
//
// DESCRIPCIÓN: Clase que describe un buffer de vértices
//
/**
 * @class GEVertexBuffer
 * @brief Class GEVertexBuffer
 */
class GEVertexBuffer
{
public:
	VkBuffer buffer;
	VkDeviceMemory memory;

	GEVertexBuffer(GEGraphicsContext* gc, size_t size, const void* data);
	/**
	 * @brief Función destroy
	 */
	void destroy(GEGraphicsContext* gc);
};

