#include "GETexture.h"

#include <stb_image.h>
#include <iostream>
#include <vulkan/vulkan.hpp>
#include "commonDebug.h"

//
// FUNCIÓN: GETexture::GETexture(GEGraphicsContext* gc, const char** filename)
//
// PROPÓSITO: Crea una textura cubemap a partir de 6 ficheros de imagen
//
GETexture::GETexture(GEGraphicsContext* gc, const char** filename)
{
	int texChannels;
	stbi_uc* pixels[6];
	pixels[0] = stbi_load(filename[0], &textureWidth, &textureHeight, &texChannels, STBI_rgb_alpha);
	pixels[1] = stbi_load(filename[1], &textureWidth, &textureHeight, &texChannels, STBI_rgb_alpha);
	pixels[2] = stbi_load(filename[2], &textureWidth, &textureHeight, &texChannels, STBI_rgb_alpha);
	pixels[3] = stbi_load(filename[3], &textureWidth, &textureHeight, &texChannels, STBI_rgb_alpha);
	pixels[4] = stbi_load(filename[4], &textureWidth, &textureHeight, &texChannels, STBI_rgb_alpha);
	pixels[5] = stbi_load(filename[5], &textureWidth, &textureHeight, &texChannels, STBI_rgb_alpha);
	VkDeviceSize layerSize = textureWidth * textureHeight * 4;
	VkDeviceSize imageSize = layerSize * 6;

	for (int i = 0; i < 6; i++) if (!pixels[i]) throw std::runtime_error("failed to load texture image!");

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createCubemapStagingBuffer(gc, &stagingBuffer, &stagingBufferMemory, layerSize, imageSize, pixels);

	VkCommandPool commandPool;
	VkQueue queue;
	createCommandPool(gc, &commandPool);
	vkGetDeviceQueue(gc->device, gc->graphicsQueueFamilyIndex, 0, &queue);

	createCubemapImage(gc);
	createImageMemory(gc);
	transitionImageLayout(gc, (uint32_t)6, commandPool, queue, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	copyBufferToImage(gc, (uint32_t)6, commandPool, queue, stagingBuffer);
	transitionImageLayout(gc, (uint32_t)6, commandPool, queue, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	vkDestroyBuffer(gc->device, stagingBuffer, nullptr);
	vkFreeMemory(gc->device, stagingBufferMemory, nullptr);

	vkDestroyCommandPool(gc->device, commandPool, nullptr);

	createCubemapImageView(gc);
	createSampler(gc);
}

//
// FUNCIÓN: GETexture::GETexture(GEGraphicsContext* gc, const char* filename)
//
// PROPÓSITO: Crea una textura a partir de un fichero de imagen
//
GETexture::GETexture(GEGraphicsContext* gc, const char* filename)
{
	int texChannels;
	GE_DEBUG_WARN("Intentando cargar textura: " << filename );
	stbi_uc* pixels = stbi_load(filename, &textureWidth, &textureHeight, &texChannels, STBI_rgb_alpha);
	VkDeviceSize imageSize = textureWidth * textureHeight * 4;

	if (!pixels) {
		GE_DEBUG_ERROR( "No se pudo encontrar el archivo: " << filename );
		throw std::runtime_error("failed to load texture image!");
	}

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createStagingBuffer(gc, &stagingBuffer, &stagingBufferMemory, imageSize, pixels);

	VkCommandPool commandPool;
	VkQueue queue;
	createCommandPool(gc, &commandPool);
	vkGetDeviceQueue(gc->device, gc->graphicsQueueFamilyIndex, 0, &queue);

	createImage(gc);
	createImageMemory(gc);
	transitionImageLayout(gc, (uint32_t)1, commandPool, queue, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	copyBufferToImage(gc, (uint32_t)1, commandPool, queue, stagingBuffer);
	transitionImageLayout(gc, (uint32_t)1, commandPool, queue, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	vkDestroyBuffer(gc->device, stagingBuffer, nullptr);
	vkFreeMemory(gc->device, stagingBufferMemory, nullptr);

	vkDestroyCommandPool(gc->device, commandPool, nullptr);

	createImageView(gc);
	createSampler(gc);
}

//
// FUNCIÓN: GETexture::destroy(GEGraphicsContext* gc)
//
// PROPÓSITO: Destruye los campos de una textura
//
void GETexture::destroy(GEGraphicsContext* gc)
{
	if (textureSampler != VK_NULL_HANDLE) {
		vkDestroySampler(gc->device, textureSampler, nullptr);
		textureSampler = VK_NULL_HANDLE;
	}
	if (textureImageView != VK_NULL_HANDLE) {
		vkDestroyImageView(gc->device, textureImageView, nullptr);
		textureImageView = VK_NULL_HANDLE;
	}
	if (textureImage != VK_NULL_HANDLE) {
		vkDestroyImage(gc->device, textureImage, nullptr);
		textureImage = VK_NULL_HANDLE;
	}
	if (textureImageMemory != VK_NULL_HANDLE) {
		vkFreeMemory(gc->device, textureImageMemory, nullptr);
		textureImageMemory = VK_NULL_HANDLE;
	}
}

//
// FUNCIÓN: GETexture::createStagingBuffer()
//
// PROPÓSITO: Crea un buffer con su memoria asociada
//
void GETexture::createStagingBuffer(GEGraphicsContext* gc, VkBuffer* buffer, VkDeviceMemory* deviceMemory, size_t size, unsigned char* pixels)
{
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(gc->device, &bufferInfo, nullptr, buffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(gc->device, *buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = gc->findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	if (vkAllocateMemory(gc->device, &allocInfo, nullptr, deviceMemory) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate buffer memory!");
	}

	vkBindBufferMemory(gc->device, *buffer, *deviceMemory, 0);

	void* data;
	vkMapMemory(gc->device, *deviceMemory, 0, size, 0, &data);
	memcpy(data, pixels, size);
	vkUnmapMemory(gc->device, *deviceMemory);
}

//
// FUNCIÓN: GETexture::createCubemapStagingBuffer()
//
// PROPÓSITO: Crea un buffer con su memoria asociada
//
void GETexture::createCubemapStagingBuffer(GEGraphicsContext* gc, VkBuffer* buffer, VkDeviceMemory* deviceMemory, size_t layerSize, size_t size, unsigned char** pixels)
{
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(gc->device, &bufferInfo, nullptr, buffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(gc->device, *buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = gc->findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	if (vkAllocateMemory(gc->device, &allocInfo, nullptr, deviceMemory) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate buffer memory!");
	}

	vkBindBufferMemory(gc->device, *buffer, *deviceMemory, 0);

	void* data;
	vkMapMemory(gc->device, *deviceMemory, 0, size, 0, &data);
	for (int i = 0; i < 6; i++) memcpy(static_cast<char*>(data) + (layerSize * i), pixels[i], static_cast<size_t>(layerSize));
	vkUnmapMemory(gc->device, *deviceMemory);
}

//
// FUNCIÓN: GETexture::createImage(GEGraphicsContext* gc) 
//
// PROPÓSITO: Crea un buffer de tipo imagen
//
void GETexture::createImage(GEGraphicsContext* gc)
{
	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = (uint32_t)textureWidth;
	imageInfo.extent.height = (uint32_t)textureHeight;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateImage(gc->device, &imageInfo, nullptr, &textureImage) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create image!");
	}
}

//
// FUNCIÓN: GETexture::createCubemapImage(GEGraphicsContext* gc) 
//
// PROPÓSITO: Crea un buffer de tipo imagen
//
void GETexture::createCubemapImage(GEGraphicsContext* gc)
{
	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = (uint32_t)textureWidth;
	imageInfo.extent.height = (uint32_t)textureHeight;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 6;
	imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
	imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateImage(gc->device, &imageInfo, nullptr, &textureImage) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create image!");
	}
}

//
// FUNCIÓN: GETexture::createImageMemory(GEGraphicsContext* gc) 
//
// PROPÓSITO: Crea la memoria asociada a un buffer de imagen
//
void GETexture::createImageMemory(GEGraphicsContext* gc)
{
	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(gc->device, textureImage, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = gc->findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	if (vkAllocateMemory(gc->device, &allocInfo, nullptr, &textureImageMemory) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate image memory!");
	}

	vkBindImageMemory(gc->device, textureImage, textureImageMemory, 0);
}

//
// FUNCIÓN: GETexture::createImageView(GEGraphicsContext* gc)
// 
// PROPÓSITO: Crea la vista asociada a una imagen
//
void GETexture::createImageView(GEGraphicsContext* gc)
{
	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = textureImage;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	if (vkCreateImageView(gc->device, &viewInfo, nullptr, &textureImageView) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create texture image view!");
	}
}

//
// FUNCIÓN: GETexture::createCubemapImageView(GEGraphicsContext* gc)
// 
// PROPÓSITO: Crea la vista asociada a una imagen
//
void GETexture::createCubemapImageView(GEGraphicsContext* gc)
{
	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = textureImage;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
	viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 6;

	if (vkCreateImageView(gc->device, &viewInfo, nullptr, &textureImageView) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create texture image view!");
	}
}

//
// FUNCIÓN: GETexture::createSampler(GEGraphicsContext* gc)
// 
// PROPÓSITO: Crea un sampler para acceder a una imagen de textura
//
void GETexture::createSampler(GEGraphicsContext* gc)
{
	VkPhysicalDeviceProperties properties = {};
	vkGetPhysicalDeviceProperties(gc->physicalDevice, &properties);

	VkSamplerCreateInfo samplerInfo = {};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

	if (vkCreateSampler(gc->device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create texture sampler!");
	}
}

//
// FUNCIÓN: GETexture::transitionImageLayout()
//
// PROPÓSITO: Modifica la definición interna de una imagen para cambiar su uso
//
void GETexture::transitionImageLayout(GEGraphicsContext* gc, uint32_t layers, VkCommandPool commandPool, VkQueue queue, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(gc->device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = textureImage;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = layers;

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else {
		throw std::invalid_argument("unsupported layout transition!");
	}

	vkCmdPipelineBarrier(
		commandBuffer,
		sourceStage, destinationStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);

	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(queue);

	vkFreeCommandBuffers(gc->device, commandPool, 1, &commandBuffer);
}

//
// FUNCIÓN: GETexture::copyBufferToImage(GEGraphicsContext* gc, VkCommandPool commandPool, VkQueue queue, VkBuffer buffer)
//
// PROPÓSITO: Copia el contenido de un buffer a un buffer de imagen
//
void GETexture::copyBufferToImage(GEGraphicsContext* gc, uint32_t layers, VkCommandPool commandPool, VkQueue queue, VkBuffer buffer)
{
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(gc->device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	VkBufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = layers;
	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = { (uint32_t)textureWidth, (uint32_t)textureHeight, 1 };

	vkCmdCopyBufferToImage(commandBuffer, buffer, textureImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(queue);

	vkFreeCommandBuffers(gc->device, commandPool, 1, &commandBuffer);
}

//
// FUNCIÓN: GETexture::createCommandPool(GEGraphicsContext* gc, VkCommandPool* commandPool)
//
// PROPÓSITO: Crea el command pool vinculado a la familia de colas para gráficos
//
void GETexture::createCommandPool(GEGraphicsContext* gc, VkCommandPool* commandPool)
{
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = gc->graphicsQueueFamilyIndex;

	if (vkCreateCommandPool(gc->device, &poolInfo, nullptr, commandPool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create command pool!");
	}
}