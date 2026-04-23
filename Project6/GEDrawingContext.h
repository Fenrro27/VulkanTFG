#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "GEGraphicsContext.h"
#include "GEWindowPosition.h"

//
// CLASE: GEDrawingContext
//
// DESCRIPCIN: Clase que contiene la informacin sobre el destino de las imgenes a generar (swapchain, vistas, colas de comandos)
//
class GEDrawingContext
{
public:
	std::vector<VkImageView> imageViews;

private:
	// Campos auxiliares
	uint32_t imageCount;
	uint32_t frameCount;
	size_t currentFrame = 0;
	uint32_t currentImage = 0;

	// Componentes grficos
	VkSwapchainKHR swapChain;
	VkFormat imageFormat;
	VkExtent2D imageExtent;
	std::vector<VkImage> images;
	VkQueue graphicsQueue;
	VkQueue presentQueue;

	// Sincronizacin entre imgenes
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;

	// compute shader, error fix
	const int MAX_FRAMES_IN_FLIGHT = 2;

public:
	GEDrawingContext(GEGraphicsContext* gc, GEWindowPosition wpos);
	void destroy(GEGraphicsContext* gc);
	void recreate(GEGraphicsContext* gc, GEWindowPosition wpos);
	VkFormat getFormat();
	VkExtent2D getExtent();
	uint32_t getImageCount();
	uint32_t getCurrentImage();

	// Mtodos de generacin de la imagen
	bool waitForNextImage(GEGraphicsContext* gc);
	void submitGraphicsCommands(GEGraphicsContext* gc, std::vector<VkCommandBuffer> commandBuffers);
	void submitPresentCommands(GEGraphicsContext* gc);
	void waitIdle(VkDevice device);

private:
	// Mtodos de creacin de componentes
	void createSwapChain(GEGraphicsContext* gc, GEWindowPosition wpos);
	void createImageViews(VkDevice device);
	void createSyncObjects(VkDevice device);
	void createQueues(GEGraphicsContext* gc);

	// Mtodos auxiliares
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GEWindowPosition wpos);


};

