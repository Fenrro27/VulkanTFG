/**
 * @file GEComputeShader.h
 * @brief Archivo GEComputeShader.h
 */
#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "GEGraphicsContext.h"
#include "GERenderingContext.h"
#include "GEParticlesSystem.h"


/**
 * @class GEComputeShader
 * @brief Class GEComputeShader
 */
class GEComputeShader
{
public:
    VkPipeline pipeline;
    VkPipelineLayout layout;
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorPool descriptorPool;
    std::vector < std::vector<VkDescriptorSet>> systemDescriptorSets;

    GEComputeShader(GEGraphicsContext* gc, int shaderResource, uint32_t imageCount);
    /**
     * @brief FunciÃ³n addParticleSystem
     */
    int addParticleSystem(GEGraphicsContext* gc, uint32_t imageCount, GEParticlesSystem* particleSystem);
    /**
     * @brief FunciÃ³n recordCommands
     */
    void recordCommands(VkCommandBuffer cb, uint32_t systemIndex, uint32_t imageIndex, uint32_t groupCountX);
    /**
     * @brief FunciÃ³n destroy
     */
    void destroy(GEGraphicsContext* gc);

private:
    /**
     * @brief FunciÃ³n createDescriptorSetLayout
     */
    void createDescriptorSetLayout(GEGraphicsContext* gc);
    /**
     * @brief FunciÃ³n createDescriptorSets
     */
    void createDescriptorSets(GEGraphicsContext* gc, uint32_t imageCount);
    /**
     * @brief FunciÃ³n createComputePipeline
     */
    void createComputePipeline(GEGraphicsContext* gc, int shaderResource);

    /**
     * @brief FunciÃ³n getFileFromResource
     */
    std::vector<char> getFileFromResource(int resource);
    /**
     * @brief FunciÃ³n createShaderModule
     */
    VkShaderModule createShaderModule(GEGraphicsContext* gc, const std::vector<char>& code);
};