/**
 * @file GEDepthBuffer.h
 * @brief Archivo GEDepthBuffer.h
 */
#pragma once

#include <vulkan/vulkan.h>
#include "GEGraphicsContext.h"

//
// CLASE: GEDepthBuffer
//
// DESCRIPCIÓN: Clase que describe un buffer de profundidad
//
/**
 * @class GEDepthBuffer
 * @brief Class GEDepthBuffer
 */
class GEDepthBuffer
{
public:
	VkImage image;
	VkDeviceMemory imageMemory;
	VkImageView imageView;

	GEDepthBuffer(GEGraphicsContext* gc, VkExtent2D extent);
	/**
	 * @brief Función destroy
	 */
	void destroy(GEGraphicsContext* gc);
};

