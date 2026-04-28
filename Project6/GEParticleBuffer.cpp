/**
 * @file GEParticleBuffer.cpp
 * @brief Archivo GEParticleBuffer.cpp
 */
#include "GEParticleBuffer.h"

#include <iostream>

//	
// compute shader
// FUNCIÓN: GEParticleBuffer::GEParticleBuffer(GEGraphicsContext* gc, size_t size, const void* data)
//
// PROPÓSITO: Crea un Particle Buffer
//
GEParticleBuffer::GEParticleBuffer(GEGraphicsContext* gc, size_t size, const void* data)
{
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = //compute shader
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | // Permite que el Graphics Pipeline lo use como entrada de vértices (Vertex Input) 
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | // Permite que el Compute Shader lea/escriba en él como un SSBO (Storage Buffer) 
		VK_BUFFER_USAGE_TRANSFER_DST_BIT;     // Permite que el buffer reciba datos desde la CPU 
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(gc->device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
	{
		/**
		 * @brief Función std::runtime_error
		 */
		throw std::runtime_error("failed to create buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(gc->device, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = gc->findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	if (vkAllocateMemory(gc->device, &allocInfo, nullptr, &memory) != VK_SUCCESS)
	{
		/**
		 * @brief Función std::runtime_error
		 */
		throw std::runtime_error("failed to allocate buffer memory!");
	}

	vkBindBufferMemory(gc->device, buffer, memory, 0);
	if (data != nullptr) {
		void* gpudata;
		vkMapMemory(gc->device, memory, 0, size, 0, &gpudata);
		memcpy(gpudata, data, size);
		vkUnmapMemory(gc->device, memory);
	}
}

//
// FUNCIÓN: GEParticleBuffer::destroy(GEGraphicsContext* gc)
//
// PROPÓSITO: Destruye los campos de un Particle Buffer
//
/**
 * @brief Función GEParticleBuffer::destroy
 */
void GEParticleBuffer::destroy(GEGraphicsContext* gc)
{
	vkDestroyBuffer(gc->device, buffer, nullptr);
	vkFreeMemory(gc->device, memory, nullptr);
}
