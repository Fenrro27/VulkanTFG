/**

 * @file GEDrawingContext.h
 * @brief Definición de la clase GEDrawingContext para la gestión de la cadena de intercambio y sincronización.
 */
#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "GEGraphicsContext.h"
#include "GEWindowPosition.h"

/**
 * @class GEDrawingContext
 * @brief Clase que gestiona los recursos necesarios para el dibujado y la presentación.
 * 
 * Incluye la cadena de intercambio (swapchain), las vistas de imagen, las colas de comandos
 * y los objetos de sincronización (semáforos y fences).
 */
class GEDrawingContext
{
public:
	std::vector<VkImageView> imageViews; /**< Vistas de imagen de la swapchain. */

private:
	// Campos auxiliares
	uint32_t imageCount;
	uint32_t frameCount;
	size_t currentFrame = 0;
	uint32_t currentImage = 0;

	// Componentes gráficos
	VkSwapchainKHR swapChain;
	VkFormat imageFormat;
	VkExtent2D imageExtent;
	std::vector<VkImage> images;
	VkQueue graphicsQueue;
	VkQueue presentQueue;

	// Sincronización entre imágenes
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;

	const int MAX_FRAMES_IN_FLIGHT = 2;

public:
	/**
	 * @brief Constructor de GEDrawingContext.
	 */
	GEDrawingContext(GEGraphicsContext* gc, GEWindowPosition wpos);

	/**
	 * @brief Libera los recursos de la swapchain y sincronización.
	 */
	void destroy(GEGraphicsContext* gc);

	/**
	 * @brief Recrea la swapchain (útil al redimensionar la ventana).
	 */
	void recreate(GEGraphicsContext* gc, GEWindowPosition wpos);

	/**
	 * @brief Obtiene el formato de imagen de la swapchain.
	 */
	VkFormat getFormat();

	/**
	 * @brief Obtiene las dimensiones de la swapchain.
	 */
	VkExtent2D getExtent();

	/**
	 * @brief Obtiene el número de imágenes en la swapchain.
	 */
	uint32_t getImageCount();

	/**
	 * @brief Obtiene el índice de la imagen actual.
	 */
	uint32_t getCurrentImage();

	/**
	 * @brief Espera a que la siguiente imagen de la swapchain esté disponible.
	 * @return true si se obtuvo la imagen correctamente.
	 */
	bool waitForNextImage(GEGraphicsContext* gc);

	/**
	 * @brief Envía los command buffers a la cola de gráficos.
	 */
	void submitGraphicsCommands(GEGraphicsContext* gc, std::vector<VkCommandBuffer> commandBuffers);

	/**
	 * @brief Presenta la imagen renderizada en la pantalla.
	 */
	void submitPresentCommands(GEGraphicsContext* gc);

	/**
	 * @brief Espera a que el dispositivo termine todas sus tareas.
	 */
	void waitIdle(VkDevice device);

private:
	/**
	 * @brief Crea la cadena de intercambio (Swap Chain).
	 */
	void createSwapChain(GEGraphicsContext* gc, GEWindowPosition wpos);

	/**
	 * @brief Crea las vistas de imagen (Image Views) para la swapchain.
	 */
	void createImageViews(VkDevice device);

	/**
	 * @brief Crea los objetos de sincronización (Semáforos y Fences).
	 */
	void createSyncObjects(VkDevice device);

	/**
	 * @brief Obtiene las colas de gráficos y presentación.
	 */
	void createQueues(GEGraphicsContext* gc);

	/**
	 * @brief Elige el formato de superficie adecuado para la swapchain.
	 */
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

	/**
	 * @brief Elige la extensión (dimensiones) adecuada para la swapchain.
	 */
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GEWindowPosition wpos);
};
