#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "GEGraphicsContext.h"
#include "GERenderingContext.h"
#include "GEParticlesSystem.h"


class GEComputeShader
{
public:
    VkPipeline pipeline;
    VkPipelineLayout layout;
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorPool descriptorPool;
    std::vector < std::vector<VkDescriptorSet>> systemDescriptorSets;

    GEComputeShader(GEGraphicsContext* gc, int shaderResource, uint32_t imageCount);
    int addParticleSystem(GEGraphicsContext* gc, uint32_t imageCount, GEParticlesSystem* particleSystem);
    void recordCommands(VkCommandBuffer cb, uint32_t systemIndex, uint32_t imageIndex, uint32_t groupCountX);
    void destroy(GEGraphicsContext* gc);

private:
    void createDescriptorSetLayout(GEGraphicsContext* gc);
    void createDescriptorSets(GEGraphicsContext* gc, uint32_t imageCount);
    void createComputePipeline(GEGraphicsContext* gc, int shaderResource);

    std::vector<char> getFileFromResource(int resource);
    VkShaderModule createShaderModule(GEGraphicsContext* gc, const std::vector<char>& code);
};