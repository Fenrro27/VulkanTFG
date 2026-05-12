/**
 * @file GECommandContext.cpp
 * @brief Archivo GECommandContext.cpp
 */
#include "GECommandContext.h"

#include <iostream>

///////////////////////////////////////////////////////////////////////////////////////////
/////                                                                                 /////
/////                               Métodos públicos                                  /////
/////                                                                                 /////
///////////////////////////////////////////////////////////////////////////////////////////

//
// FUNCIÓN: GECommandContext::GECommandContext(GEGraphicsContext* gc, uint32_t imageCount)
//
// PROPÓSITO: Construye los buffers de comnandos
//
GECommandContext::GECommandContext(GEGraphicsContext* gc, uint32_t imageCount)
{
	createCommandPool(gc);
	createCommandBuffers(gc, imageCount);
}

//
// FUNCIÓN: GECommandContext::destroy(GEGraphicsContext* gc)
//
// PROPÓSITO: Destruye los buffers de comnandos
//
/**
 * @brief Función GECommandContext::destroy
 */
void GECommandContext::destroy(GEGraphicsContext* gc)
{
	size_t bufferCount = commandBuffers.size();

	vkFreeCommandBuffers(gc->device, commandPool, (uint32_t) bufferCount, commandBuffers.data());
	vkDestroyCommandPool(gc->device, commandPool, nullptr);
}

///////////////////////////////////////////////////////////////////////////////////////////
/////                                                                                 /////
/////                     Métodos de creación de los componentes                      /////
/////                                                                                 /////
///////////////////////////////////////////////////////////////////////////////////////////

//
// FUNCIÓN: GECommandContext::createCommandPool(GEGraphicsContext* gc)
//
// PROPÓSITO: Crea el command pool vinculado a la familia de colas para gráficos
//
/**
 * @brief Función GECommandContext::createCommandPool
 */
void GECommandContext::createCommandPool(GEGraphicsContext* gc)
{
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = gc->graphicsQueueFamilyIndex;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;


	if (vkCreateCommandPool(gc->device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
	{
		/**
		 * @brief Función std::runtime_error
		 */
		throw std::runtime_error("failed to create command pool!");
	}
}

//
// FUNCIÓN: GECommandContext::createCommandBuffers(GEGraphicsContext* gc, uint32_t imageCount)
//
// PROPÓSITO: Crea los buffers de comandos que se enviarán a la cola gráfica
//            El contenido de los buffers incluye la orden de dibujar.
//
/**
 * @brief Función GECommandContext::createCommandBuffers
 */
void GECommandContext::createCommandBuffers(GEGraphicsContext* gc, uint32_t imageCount)
{
	commandBuffers.resize(imageCount);

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = imageCount;

	if (vkAllocateCommandBuffers(gc->device, &allocInfo, commandBuffers.data()) != VK_SUCCESS)
	{
		/**
		 * @brief Función std::runtime_error
		 */
		throw std::runtime_error("failed to allocate command buffers!");
	}
}

//
// FUNCIÓN: GECommandContext::beginCommandBuffers()
//
// PROPÓSITO: Comienza a rellenar los command buffers
//
/**
 * @brief Función GECommandContext::beginCommandBuffers
 */
void GECommandContext::beginCommandBuffers()
{
	for(auto& commandBuffer : commandBuffers)
	{

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		{
			/**
			 * @brief Función std::runtime_error
			 */
			throw std::runtime_error("failed to begin recording command buffer!");
		}
	}
}

//
// FUNCIÓN: GECommandContext::endCommandBuffers()
//
// PROPÓSITO: Termina de rellenar los command buffers
//
/**
 * @brief Función GECommandContext::endCommandBuffers
 */
void GECommandContext::endCommandBuffers()
{
	for (auto& commandBuffer : commandBuffers)
	{
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		{
			/**
			 * @brief Función std::runtime_error
			 */
			throw std::runtime_error("failed to record command buffer!");
		}
	}
}