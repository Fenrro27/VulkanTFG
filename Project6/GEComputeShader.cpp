#include "GEComputeShader.h"
#include <stdexcept>
#include <windows.h>

GEComputeShader::GEComputeShader(GEGraphicsContext* gc, int shaderResource, uint32_t imageCount){//VkBuffer bufferA, VkBuffer bufferB, size_t bufferSize) {
    createDescriptorSetLayout(gc);
    createDescriptorSets(gc, imageCount);
    createComputePipeline(gc, shaderResource);
}

void GEComputeShader::createDescriptorSetLayout(GEGraphicsContext* gc) {
    // Configurar el Pool para manejar el doble de descriptores STORAGE_BUFFER
    
    // Binding 0: Buffer de lectura
    VkDescriptorSetLayoutBinding bindingIn = {};
    bindingIn.binding = 0;
    bindingIn.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    bindingIn.descriptorCount = 1;
    bindingIn.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    // Binding 1: Buffer de escritura
    VkDescriptorSetLayoutBinding bindingOut = {};
    bindingOut.binding = 1;
    bindingOut.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    bindingOut.descriptorCount = 1;
    bindingOut.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    VkDescriptorSetLayoutBinding bindingParams = {};
    bindingParams.binding = 2;
    bindingParams.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    bindingParams.descriptorCount = 1;
    bindingParams.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;



    // Agrupamos ambos en un array para crear el Layout
    VkDescriptorSetLayoutBinding bindings[] = { bindingIn, bindingOut, bindingParams };
    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO; 
    layoutInfo.bindingCount = 3; 
    layoutInfo.pBindings = bindings;


    if (vkCreateDescriptorSetLayout(gc->device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create compute descriptor set layout!"); 
    }
}

// sirve para poder trasferir dagtos entre cpu y gpu
void GEComputeShader::createDescriptorSets(GEGraphicsContext* gc, uint32_t imageCount) {
    /*/ Crear Pool
    VkDescriptorPoolSize poolSize[2] = {};
    poolSize[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSize[0].descriptorCount = imageCount * 2;
    poolSize[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize[1].descriptorCount = imageCount;

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 2;
    poolInfo.pPoolSizes = poolSize;
    poolInfo.maxSets = imageCount; 

        if (vkCreateDescriptorPool(gc->device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create compute descriptor pool!");
        }

    // Reservar Sets
    std::vector<VkDescriptorSetLayout> layouts(imageCount, descriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = imageCount;
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(imageCount);
    if (vkAllocateDescriptorSets(gc->device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate compute descriptor sets!"); 
    }

    // Actualizar Sets con la LÓGICA PING-PONG
    for (size_t i = 0; i < imageCount; i++) {
        // Alternamos cuál es entrada (binding 0) y cuál es salida (binding 1)
        VkBuffer currentIn = (i % 2 == 0) ? particleSystem->getParticlesBufferA()->buffer : particleSystem->getParticlesBufferB()->buffer;
        VkBuffer currentOut = (i % 2 == 0) ? particleSystem->getParticlesBufferB()->buffer : particleSystem->getParticlesBufferA()->buffer;
        size_t bufferSize = particleSystem->getParticlesSize();

        VkDescriptorBufferInfo infoIn = { currentIn, 0, bufferSize };
        VkDescriptorBufferInfo infoOut = { currentOut, 0, bufferSize };

        // Buffer de parámetros
        VkDescriptorBufferInfo infoParams = {};
        infoParams.buffer = particleSystem->getEmitterParamsBuffer()->buffers[i];
        infoParams.offset = 0;
        infoParams.range = sizeof(GEEmitterParams);

        // Preparamos dos escrituras por cada set 
        VkWriteDescriptorSet descriptorWrites[3] = {};

        // Binding 0: Entrada
        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = descriptorSets[i];
        descriptorWrites[0].dstBinding = 0; 
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &infoIn;

        // Binding 1: Salida
        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = descriptorSets[i];
        descriptorWrites[1].dstBinding = 1; 
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pBufferInfo = &infoOut;

		//Binding 2: Parámetros
        descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[2].dstSet = descriptorSets[i];
        descriptorWrites[2].dstBinding = 2;
        descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[2].descriptorCount = 1;
        descriptorWrites[2].pBufferInfo = &infoParams;
        

        vkUpdateDescriptorSets(gc->device, 3, descriptorWrites, 0, nullptr);
    }*/

    VkDescriptorPoolSize poolSize[2] = {};
    poolSize[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSize[0].descriptorCount = imageCount * 2 * 10;
    poolSize[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize[1].descriptorCount = imageCount * 10;

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 2;
    poolInfo.pPoolSizes = poolSize;
    poolInfo.maxSets = imageCount * 10;

    if (vkCreateDescriptorPool(gc->device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create compute descriptor pool!");
    }

}



int GEComputeShader::addParticleSystem(GEGraphicsContext* gc, uint32_t imageCount, GEParticlesSystem* particleSystem) {
   
    // Reservar Sets
    std::vector<VkDescriptorSetLayout> layouts(imageCount, descriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = imageCount;
    allocInfo.pSetLayouts = layouts.data();

    std::vector<VkDescriptorSet> newDescriptorSets(imageCount);

    if (vkAllocateDescriptorSets(gc->device, &allocInfo, newDescriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate compute descriptor sets!");
    }

    // Actualizar Sets con la LÓGICA PING-PONG
    for (size_t i = 0; i < imageCount; i++) {
        // Alternamos cuál es entrada (binding 0) y cuál es salida (binding 1)
        VkBuffer currentIn = (i % 2 == 0) ? particleSystem->getParticlesBufferA()->buffer : particleSystem->getParticlesBufferB()->buffer;
        VkBuffer currentOut = (i % 2 == 0) ? particleSystem->getParticlesBufferB()->buffer : particleSystem->getParticlesBufferA()->buffer;
        size_t bufferSize = particleSystem->getParticlesSize();

        VkDescriptorBufferInfo infoIn = { currentIn, 0, bufferSize };
        VkDescriptorBufferInfo infoOut = { currentOut, 0, bufferSize };

        // Buffer de parámetros
        VkDescriptorBufferInfo infoParams = {};
        infoParams.buffer = particleSystem->getEmitterParamsBuffer()->buffers[i];
        infoParams.offset = 0;
        infoParams.range = sizeof(GEEmitterParams);

        // Preparamos dos escrituras por cada set
        VkWriteDescriptorSet descriptorWrites[3] = {};

        // Binding 0: Entrada
        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = newDescriptorSets[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &infoIn;

        // Binding 1: Salida
        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = newDescriptorSets[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pBufferInfo = &infoOut;

        //Binding 2: Parámetros
        descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[2].dstSet = newDescriptorSets[i];
        descriptorWrites[2].dstBinding = 2;
        descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[2].descriptorCount = 1;
        descriptorWrites[2].pBufferInfo = &infoParams;


        vkUpdateDescriptorSets(gc->device, 3, descriptorWrites, 0, nullptr);
    }

    systemDescriptorSets.push_back(newDescriptorSets);
	return static_cast<int>(systemDescriptorSets.size() - 1);

}

void GEComputeShader::createComputePipeline(GEGraphicsContext* gc, int shaderResource) {
    // Pipeline Layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;

    if (vkCreatePipelineLayout(gc->device, &pipelineLayoutInfo, nullptr, &layout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create compute pipeline layout!"); 
    }

    // Cargar Shader
    std::vector<char> shaderCode = getFileFromResource(shaderResource); 
        VkShaderModule shaderModule = createShaderModule(gc, shaderCode); 

        VkComputePipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipelineInfo.layout = layout;
    pipelineInfo.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    pipelineInfo.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    pipelineInfo.stage.module = shaderModule;
    pipelineInfo.stage.pName = "main";

    if (vkCreateComputePipelines(gc->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create compute pipeline!");
    }

    vkDestroyShaderModule(gc->device, shaderModule, nullptr);
}

void GEComputeShader::recordCommands(VkCommandBuffer cb,uint32_t systemIndex ,uint32_t imageIndex, uint32_t groupCountX) {
    vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
        vkCmdBindDescriptorSets(cb, VK_PIPELINE_BIND_POINT_COMPUTE, layout, 0, 1, &systemDescriptorSets[systemIndex][imageIndex], 0, nullptr);
        vkCmdDispatch(cb, groupCountX, 1, 1);
}

void GEComputeShader::destroy(GEGraphicsContext* gc) {
    vkDestroyPipeline(gc->device, pipeline, nullptr); 
        vkDestroyPipelineLayout(gc->device, layout, nullptr); 
        vkDestroyDescriptorPool(gc->device, descriptorPool, nullptr);
        vkDestroyDescriptorSetLayout(gc->device, descriptorSetLayout, nullptr);
}

std::vector<char> GEComputeShader::getFileFromResource(int resource) {
    HRSRC shaderHandle = FindResource(NULL, MAKEINTRESOURCE(resource), RT_HTML);
        HGLOBAL shaderGlobal = LoadResource(NULL, shaderHandle); 
        LPCTSTR shaderPtr = static_cast<LPCTSTR>(LockResource(shaderGlobal)); 
        DWORD shaderSize = SizeofResource(NULL, shaderHandle); 
        std::vector<char> shader(shaderSize);
    memcpy(shader.data(), shaderPtr, shaderSize);
    UnlockResource(shaderGlobal);
        return shader;
}

VkShaderModule GEComputeShader::createShaderModule(GEGraphicsContext* gc, const std::vector<char>& code) {
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO; 
        createInfo.codeSize = code.size(); 
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data()); 
        VkShaderModule shaderModule;
    vkCreateShaderModule(gc->device, &createInfo, nullptr, &shaderModule); 
        return shaderModule;
}