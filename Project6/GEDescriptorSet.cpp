#include "GEDescriptorSet.h"

#include "GEUniformBuffer.h"
#include "GETexture.h"
#include <iostream>

//
// FUNCI�N:GEDescriptorSet::GEDescriptorSet(GEGraphicsContext* gc, std::vector<GEUniformBuffer> ubos)
// 
// PROP�SITO: Crea los conjuntos de descriptores asociados a los buffers
//
GEDescriptorSet::GEDescriptorSet(GEGraphicsContext* gc, GERenderingContext* rc, std::vector<GEUniformBuffer*> ubos, std::vector<GETexture*> tex)
{

	uint32_t bufferCount = (uint32_t)ubos.size();
	uint32_t textureCount = (uint32_t)tex.size();
	uint32_t imageCount = rc->imageCount;

	std::vector<VkDescriptorPoolSize> poolSizes(bufferCount + textureCount);

	for (uint32_t i = 0; i < bufferCount; i++)
	{
		VkDescriptorPoolSize poolSize = {};
		poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSize.descriptorCount = imageCount;
		poolSizes[i] = poolSize;
	}
	for (uint32_t i = 0; i < textureCount; i++)
	{
		VkDescriptorPoolSize poolSize = {};
		poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSize.descriptorCount = imageCount;
		poolSizes[bufferCount + i] = poolSize;
	}

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = (uint32_t)poolSizes.size();
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = imageCount;

	if (vkCreateDescriptorPool(gc->device, &poolInfo, nullptr, &this->descriptorPool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor pool!");
	}

	std::vector<VkDescriptorSetLayout> layouts(imageCount, rc->getActiveDescriptorSetLayout());
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = this->descriptorPool;
	allocInfo.descriptorSetCount = imageCount;
	allocInfo.pSetLayouts = layouts.data();

	descriptorSets.resize(imageCount);

	if (vkAllocateDescriptorSets(gc->device, &allocInfo, descriptorSets.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	for (size_t i = 0; i < imageCount; i++)
	{
		std::vector<VkDescriptorBufferInfo> buffersInfo;
		buffersInfo.resize(bufferCount);

		for (uint32_t j = 0; j < bufferCount; j++)
		{
			buffersInfo[j] = {};
			buffersInfo[j].buffer = ubos[j]->buffers[i];
			buffersInfo[j].offset = 0;
			buffersInfo[j].range = ubos[j]->bufferSize;
		}

		std::vector<VkDescriptorImageInfo> imageInfo;
		imageInfo.resize(textureCount);

		for (uint32_t j = 0; j < textureCount; j++)
		{
			imageInfo[j] = {};
			imageInfo[j].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo[j].imageView = tex[j]->textureImageView;
			imageInfo[j].sampler = tex[j]->textureSampler;
		}

		std::vector<VkWriteDescriptorSet> descriptorWrites;

		for (uint32_t j = 0; j < bufferCount; j++) {
			VkWriteDescriptorSet wubo = {};
			wubo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			wubo.dstSet = this->descriptorSets[i];
			wubo.dstBinding = j; // Asigna binding 0, luego 1, luego 2...
			wubo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			wubo.descriptorCount = 1;
			wubo.pBufferInfo = &buffersInfo[j];
			descriptorWrites.push_back(wubo);
		}

		if (textureCount > 0) {
			VkWriteDescriptorSet wtex = {};
			wtex.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			wtex.dstSet = this->descriptorSets[i];
			wtex.dstBinding = bufferCount; // La textura va al binding después de los UBOs
			wtex.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			wtex.descriptorCount = textureCount;
			wtex.pImageInfo = imageInfo.data();
			descriptorWrites.push_back(wtex);
		}

		vkUpdateDescriptorSets(gc->device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		
	}
}

//
// FUNCI�N: GEDescriptorSet::destroy(GEGraphicsContext* gc) 
//
// PROP�SITO: Destruye los conjuntos de descriptores
//
void GEDescriptorSet::destroy(GEGraphicsContext* gc)
{
	// Esta línea es redundante si se va a destruir el pool justo después
	//vkFreeDescriptorSets(gc->device, descriptorPool, (uint32_t)descriptorSets.size(), descriptorSets.data());
	vkDestroyDescriptorPool(gc->device, descriptorPool, nullptr);
}
