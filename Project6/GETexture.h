#pragma once

#include <vulkan/vulkan.h>
#include "GEGraphicsContext.h"

//
// CLASE: GETexture
//
// DESCRIPCIÓN: Clase que describe un buffer de textura y su sampler
//
class GETexture
{
public:
	VkImage textureImage;
	VkDeviceMemory textureImageMemory;
	VkImageView textureImageView;
	VkSampler textureSampler;
	int textureWidth;
	int textureHeight;

	GETexture(GEGraphicsContext* gc, const char* filename);
	GETexture(GEGraphicsContext* gc, const char** filename);
	void destroy(GEGraphicsContext* gc);

private:
	void createStagingBuffer(GEGraphicsContext* gc, VkBuffer* buffer, VkDeviceMemory* deviceMemory, size_t size, unsigned char* pixels);
	void createCubemapStagingBuffer(GEGraphicsContext* gc, VkBuffer* buffer, VkDeviceMemory* deviceMemory, size_t layersize, size_t size, unsigned char** pixels);
	void createImage(GEGraphicsContext* gc);
	void createCubemapImage(GEGraphicsContext* gc);
	void createImageMemory(GEGraphicsContext* gc);
	void createImageView(GEGraphicsContext* gc);
	void createCubemapImageView(GEGraphicsContext* gc);
	void createSampler(GEGraphicsContext* gc);
	void transitionImageLayout(GEGraphicsContext* gc, uint32_t layers, VkCommandPool commandPool, VkQueue queue, VkImageLayout oldLayout, VkImageLayout newLayout);
	void copyBufferToImage(GEGraphicsContext* gc, uint32_t layers, VkCommandPool commandPool, VkQueue queue, VkBuffer buffer);
	void createCommandPool(GEGraphicsContext* gc, VkCommandPool* commandPool);
};

