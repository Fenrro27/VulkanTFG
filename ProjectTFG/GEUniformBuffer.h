/**
 * @file GEUniformBuffer.h
 * @brief Archivo GEUniformBuffer.h
 */
#pragma once

#include "GEGraphicsContext.h"
#include <vulkan/vulkan.h>
#include <vector>

//
// CLASE: GEUniformBuffer
//
// DESCRIPCIÓN: Clase que describe un conjunto de buffers para almacenar variables uniformes 
//
/**
 * @class GEUniformBuffer
 * @brief Class GEUniformBuffer
 */
class GEUniformBuffer
{
public:
	size_t bufferSize;
	std::vector<VkBuffer> buffers;
	std::vector<VkDeviceMemory> memories;

	GEUniformBuffer(GEGraphicsContext* gc, uint32_t imageCount, size_t bufferSize);
	/**
	 * @brief Función update
	 */
	void update(GEGraphicsContext* gc, uint32_t currentImage, size_t size, const void* data);
	/**
	 * @brief Función destroy
	 */
	void destroy(GEGraphicsContext* gc);
};

