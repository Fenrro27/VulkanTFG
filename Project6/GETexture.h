/**
 * @file GETexture.h
 * @brief DefiniciÃ³n de la clase GETexture para la gestiÃ³n de texturas en Vulkan.
 */
#pragma once

#include <vulkan/vulkan.h>
#include "GEGraphicsContext.h"

/**
 * @class GETexture
 * @brief Clase que gestiona un recurso de textura, incluyendo su imagen, memoria, vista y sampler.
 * 
 * Esta clase proporciona mÃ©todos para cargar texturas desde archivos (incluyendo cubemaps)
 * y realizar las transiciones de layout necesarias en Vulkan.
 */
class GETexture
{
public:
	VkImage textureImage;         /**< Manejador de la imagen de Vulkan. */
	VkDeviceMemory textureImageMemory; /**< Memoria dedicada a la textura. */
	VkImageView textureImageView; /**< Vista de la imagen de la textura. */
	VkSampler textureSampler;     /**< Sampler para el filtrado de la textura. */
	int textureWidth;             /**< Ancho de la textura en pÃ­xeles. */
	int textureHeight;            /**< Alto de la textura en pÃ­xeles. */

	/**
	 * @brief Constructor para cargar una textura 2D desde un archivo.
	 * @param gc Contexto de grÃ¡ficos.
	 * @param filename Ruta al archivo de imagen.
	 */
	GETexture(GEGraphicsContext* gc, const char* filename);

	/**
	 * @brief Constructor para cargar un cubemap desde mÃºltiples archivos.
	 * @param gc Contexto de grÃ¡ficos.
	 * @param filename Array de rutas a los archivos de imagen (6 para un cubemap).
	 */
	GETexture(GEGraphicsContext* gc, const char** filename);

	/**
	 * @brief Libera los recursos de Vulkan asociados a la textura.
	 * @param gc Contexto de grÃ¡ficos.
	 */
	void destroy(GEGraphicsContext* gc);

private:
	/**
	 * @brief Crea un staging buffer para transferir datos de pÃ­xeles a la memoria de la GPU.
	 */
	void createStagingBuffer(GEGraphicsContext* gc, VkBuffer* buffer, VkDeviceMemory* deviceMemory, size_t size, unsigned char* pixels);
	
	/**
	 * @brief Crea un staging buffer especÃ­fico para cubemaps.
	 */
	void createCubemapStagingBuffer(GEGraphicsContext* gc, VkBuffer* buffer, VkDeviceMemory* deviceMemory, size_t layersize, size_t size, unsigned char** pixels);
	
	/**
	 * @brief Crea el objeto VkImage para una textura 2D.
	 */
	void createImage(GEGraphicsContext* gc);
	
	/**
	 * @brief Crea el objeto VkImage especÃ­fico para un cubemap.
	 */
	void createCubemapImage(GEGraphicsContext* gc);
	
	/**
	 * @brief Reserva y asigna memoria para la imagen de la textura.
	 */
	void createImageMemory(GEGraphicsContext* gc);
	
	/**
	 * @brief Crea la vista de la imagen (VkImageView) para una textura 2D.
	 */
	void createImageView(GEGraphicsContext* gc);
	
	/**
	 * @brief Crea la vista de la imagen especÃ­fica para un cubemap.
	 */
	void createCubemapImageView(GEGraphicsContext* gc);
	
	/**
	 * @brief Crea el sampler para configurar el filtrado y direccionamiento de la textura.
	 */
	void createSampler(GEGraphicsContext* gc);
	
	/**
	 * @brief Realiza la transiciÃ³n del layout de la imagen (e.g., de UNDEFINED a SHADER_READ_ONLY).
	 */
	void transitionImageLayout(GEGraphicsContext* gc, uint32_t layers, VkCommandPool commandPool, VkQueue queue, VkImageLayout oldLayout, VkImageLayout newLayout);
	
	/**
	 * @brief Copia el contenido del staging buffer a la imagen de la textura.
	 */
	void copyBufferToImage(GEGraphicsContext* gc, uint32_t layers, VkCommandPool commandPool, VkQueue queue, VkBuffer buffer);
	
	/**
	 * @brief Crea un command pool temporal para operaciones de transferencia.
	 */
	void createCommandPool(GEGraphicsContext* gc, VkCommandPool* commandPool);
};
