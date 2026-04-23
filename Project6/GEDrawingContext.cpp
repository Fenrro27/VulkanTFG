#include "GEDrawingContext.h"
#include "GEGraphicsContext.h"
#include <glm/common.hpp>
#include <iostream>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////////
/////                                                                                 /////
/////                               M�todos p�blicos                                  /////
/////                                                                                 /////
///////////////////////////////////////////////////////////////////////////////////////////

//
// FUNCI�N: GEDrawingContext::GEDrawingContext(GEGraphicsContext* gc)
//
// PROP�SITO: Crea el contexto de dibujo (im�genes y cadena de intercambio)
//
GEDrawingContext::GEDrawingContext(GEGraphicsContext* gc, GEWindowPosition wpos)
{
	createSwapChain(gc, wpos);
	createImageViews(gc->device);
	createSyncObjects(gc->device);
	createQueues(gc);
}

//
// FUNCI�N: GEDrawingContext::destroy(GEGraphicsContext* gc)
//
// PROP�SITO: Destruye los componentes del contexto de dibujo
//
void GEDrawingContext::destroy(GEGraphicsContext* gc)
{
	// Destruir objetos que dependen de frameCount
	for (size_t i = 0; i < frameCount; i++) {
		vkDestroySemaphore(gc->device, imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(gc->device, inFlightFences[i], nullptr);
	}

	for (uint32_t i = 0; i < imageCount; i++) {
		vkDestroySemaphore(gc->device, renderFinishedSemaphores[i], nullptr);
		vkDestroyImageView(gc->device, imageViews[i], nullptr);
	}

	vkDestroySwapchainKHR(gc->device, swapChain, nullptr);
}
//
// FUNCI�N: GEDrawingContext::recreate(GEGraphicsContext* gc,  GEWindowPosition wpos)
//
// PROP�SITO: Reconstruye los componentes del contexto de dibujo
//
void GEDrawingContext::recreate(GEGraphicsContext* gc, GEWindowPosition wpos)
{
	for (uint32_t i = 0; i < imageCount; i++)
	{
		vkDestroyImageView(gc->device, imageViews[i], nullptr);
	}
	vkDestroySwapchainKHR(gc->device, swapChain, nullptr);

	createSwapChain(gc, wpos);
	createImageViews(gc->device);
}

//
// FUNCI�N: GEDrawingContext::getFormat()
//
// PROP�SITO: Obtiene el formato de las im�genes
//
VkFormat GEDrawingContext::getFormat()
{
	return imageFormat;
}

//
// FUNCI�N: GEDrawingContext::getExtent()
//
// PROP�SITO: Obtiene el tama�o de las im�genes
//
VkExtent2D GEDrawingContext::getExtent()
{
	return imageExtent;
}

//
// FUNCI�N: GEDrawingContext::getImageCount()
//
// PROP�SITO: Obtiene el n�mero de im�genes
//
uint32_t GEDrawingContext::getImageCount()
{
	return imageCount;
}

//
// FUNCI�N: GEDrawingContext::getCurrentImage()
//
// PROP�SITO: Obtiene el �ndice de la imagen a generar
//
uint32_t GEDrawingContext::getCurrentImage()
{
	return currentImage;
}

///////////////////////////////////////////////////////////////////////////////////////////
/////                                                                                 /////
/////                     M�todos de creaci�n de los componentes                      /////
/////                                                                                 /////
///////////////////////////////////////////////////////////////////////////////////////////

//
// FUNCI�N: GEDrawingContext::createSwapChain(GEGraphicsContext* gc, GEWindowPosition wpos)
//
// PROP�SITO: Crea los buffers de intercambio de im�genes, el vector de im�genes
//            y sus formatos y tama�os
//
void GEDrawingContext::createSwapChain(GEGraphicsContext* gc, GEWindowPosition wpos)
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gc->physicalDevice, gc->surface, &capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(gc->physicalDevice, gc->surface, &formatCount, nullptr);
	if (formatCount != 0)
	{
		formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(gc->physicalDevice, gc->surface, &formatCount, formats.data());
	}

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(formats);
	VkExtent2D extent = chooseSwapExtent(capabilities, wpos);

	imageCount = 4;
	if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
	{
		imageCount = capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = gc->surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	if (gc->graphicsQueueFamilyIndex != gc->presentQueueFamilyIndex)
	{
		std::vector<uint32_t> queueFamilyIndices;
		queueFamilyIndices.resize(2);
		queueFamilyIndices.push_back(gc->graphicsQueueFamilyIndex);
		queueFamilyIndices.push_back(gc->presentQueueFamilyIndex);

		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
	}
	else
	{
		std::vector<uint32_t> queueFamilyIndices;
		queueFamilyIndices.resize(1);
		queueFamilyIndices.push_back(gc->graphicsQueueFamilyIndex);

		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 1;
		createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
	}

	createInfo.preTransform = capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
	createInfo.clipped = VK_TRUE;

	if (vkCreateSwapchainKHR(gc->device, &createInfo, nullptr, &swapChain) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(gc->device, swapChain, &imageCount, nullptr);
	images.resize(imageCount);
	vkGetSwapchainImagesKHR(gc->device, swapChain, &imageCount, images.data());

	imageFormat = surfaceFormat.format;
	imageExtent = extent;
}

//
// FUNCI�N: GEDrawingContext::createImageViews()
//
// PROP�SITO: Crea una vista para cada imagen de la cadena de intercambio
//
void GEDrawingContext::createImageViews(VkDevice device)
{
	imageViews.resize(imageCount);

	for (size_t i = 0; i < imageCount; i++)
	{
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = images[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = imageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(device, &createInfo, nullptr, &imageViews[i])
			!= VK_SUCCESS)
		{
			throw std::runtime_error("failed to create image views!");
		}
	}
}

//
// FUNCI�N: GEDrawingContext::createSyncObjects(VkDevice device)
//
// PROP�SITO: Crea los sem�foros y los fences para no sobreescribir las im�genes
//
void GEDrawingContext::createSyncObjects(VkDevice device)
{
	this->frameCount = MAX_FRAMES_IN_FLIGHT;

	imageAvailableSemaphores.resize(frameCount);
	
	renderFinishedSemaphores.resize(imageCount);
	inFlightFences.resize(frameCount);
	imagesInFlight.resize(imageCount, VK_NULL_HANDLE);

	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < frameCount; i++) {
		vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]);
		vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]);
	}
	for (size_t i = 0; i < imageCount; i++) {
		vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]);
	}
}

//
// FUNCI�N: GEDrawingContext::createQueues(GEGraphicsContext* gc)
//
// PROP�SITO: Obtiene las colas del dispositivo para enviar los comandos de generaci�n y presentaci�n de los gr�ficos
//
void GEDrawingContext::createQueues(GEGraphicsContext* gc)
{
	vkGetDeviceQueue(gc->device, gc->graphicsQueueFamilyIndex, 0, &graphicsQueue);
	vkGetDeviceQueue(gc->device, gc->presentQueueFamilyIndex, 0, &presentQueue);
}

///////////////////////////////////////////////////////////////////////////////////////////
/////                                                                                 /////
/////                       M�todos de generaci�n de la imagen                        /////
/////                                                                                 /////
///////////////////////////////////////////////////////////////////////////////////////////

//
// FUNCI�N: GEDrawingContext::waitForNextImage(GEGraphicsContext* gc)
//
// PROP�SITO: Espera hasta que la pr�xima imagen est� lista para ser generada
//
bool GEDrawingContext::waitForNextImage(GEGraphicsContext* gc)
{
	vkWaitForFences(gc->device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(gc->device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
		return false;
	} else if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	currentImage = imageIndex;

	if (imagesInFlight[currentImage] != VK_NULL_HANDLE) {
		vkWaitForFences(gc->device, 1, &imagesInFlight[currentImage], VK_TRUE, UINT64_MAX);
	}
	imagesInFlight[currentImage] = inFlightFences[currentFrame];

	return true;
}

//
// FUNCIN: GEDrawingContext::submitGraphicsCommands(GEGraphicsContext* gc, std::vector<VkCommandBuffer> commandBuffers)
//
// PROPSITO: Enva los comandos grficos al dispositivo
//
void GEDrawingContext::submitGraphicsCommands(GEGraphicsContext* gc, std::vector<VkCommandBuffer> commandBuffers)
{
	VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	
	VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentImage] };

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[currentImage];
	submitInfo.signalSemaphoreCount = 1;

	submitInfo.pSignalSemaphores = signalSemaphores;

	vkResetFences(gc->device, 1, &inFlightFences[currentFrame]);

	VkResult result = vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]);

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit draw command buffer!");
	}
}

//
// FUNCI�N: GEDrawingContext::submitPresentCommands(GEGraphicsContext* gc)
//
// PROP�SITO: Env�a los comandos de presentaci�n al dispositivo
//
void GEDrawingContext::submitPresentCommands(GEGraphicsContext* gc)
{
	VkSemaphore waitSemaphores[] = { renderFinishedSemaphores[currentImage] };
	VkSwapchainKHR swapChains[] = { swapChain };

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = waitSemaphores;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &currentImage;

	VkResult result = vkQueuePresentKHR(presentQueue, &presentInfo);

	if (result != VK_ERROR_OUT_OF_DATE_KHR && result != VK_SUBOPTIMAL_KHR && result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to present swap chain image!");
	}

	currentFrame = (currentFrame + 1) % frameCount;
}

///////////////////////////////////////////////////////////////////////////////////////////
/////                                                                                 /////
/////                              M�todos auxiliares                                 /////
/////                                                                                 /////
///////////////////////////////////////////////////////////////////////////////////////////


//
// FUNCI�N: GEDrawingContext::chooseSwapSurfaceFormat()
//
// PROP�SITO: Escoge el formato de imagen entre los soportados por la superficie
//
VkSurfaceFormatKHR GEDrawingContext::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	for (const VkSurfaceFormatKHR& availableFormat : availableFormats)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return availableFormat;
		}
	}

	return availableFormats[0];
}

//
// FUNCI�N: GEDrawingContext::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GEWindowPosition wpos)
//
// PROP�SITO: Escoge el tama�o de las im�genes asegurando que puede ser soportado por
//            la superficie
//
VkExtent2D GEDrawingContext::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GEWindowPosition wpos)
{
	if (capabilities.currentExtent.width != UINT32_MAX)
	{
		return capabilities.currentExtent;
	}
	else
	{
		VkExtent2D actualExtent = { };
		if (wpos.fullScreen) 
		{
			actualExtent.width = wpos.screenWidth;
			actualExtent.height = wpos.screenHeight;
		}
		else
		{
			actualExtent.width = wpos.width;
			actualExtent.height = wpos.height;
		}


		actualExtent.width = glm::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = glm::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}
}


void GEDrawingContext::waitIdle(VkDevice device) {
	vkDeviceWaitIdle(device);
}