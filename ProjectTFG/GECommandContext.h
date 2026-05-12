/**
 * @file GECommandContext.h
 * @brief Archivo GECommandContext.h
 */
#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "GEGraphicsContext.h"

//
// CLASE: GECommandContext
//
// DESCRIPCIÓN: Clase que almacena los buffers de comandos vinculados a cada imagen de la swapchain
//
/**
 * @class GECommandContext
 * @brief Class GECommandContext
 */
class GECommandContext
{
public:
	std::vector<VkCommandBuffer> commandBuffers;

	GECommandContext(GEGraphicsContext* gc, uint32_t imageCount);
	/**
	 * @brief Función destroy
	 */
	void destroy(GEGraphicsContext* gc);

	/**
	 * @brief Función beginCommandBuffers
	 */
	void beginCommandBuffers();
	/**
	 * @brief Función endCommandBuffers
	 */
	void endCommandBuffers();

private:
	VkCommandPool commandPool;
	
	// Métodos de inicialización 
	/**
	 * @brief Función createCommandPool
	 */
	void createCommandPool(GEGraphicsContext* gc);
	/**
	 * @brief Función createCommandBuffers
	 */
	void createCommandBuffers(GEGraphicsContext* gc, uint32_t imageCount);
};

