#pragma once

#include "GEGraphicsContext.h"
#include "GERenderingContext.h"
#include "GESkyboxVertex.h"
#include "GESkyboxTransform.h"
#include "GEVertexBuffer.h"
#include "GEIndexBuffer.h"
#include "GEUniformBuffer.h"
#include "GEDescriptorSet.h"
#include "GETexture.h"
#include <glm/glm.hpp>
#include <vector>

//
// CLASE: GESkybox
//
// DESCRIPCIÓN: Clase que describe una imagen de contorno
//
class GESkybox
{
protected:
	std::vector<GESkyboxVertex> vertices;
	std::vector<uint16_t> indices;
	std::unique_ptr<GETexture> cubemap;

public:
	GESkybox(GEGraphicsContext* gc, GERenderingContext* rc);
	void destroy(GEGraphicsContext* gc);
	void addCommands(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, int index);
	void update(GEGraphicsContext* gc, uint32_t index, glm::mat4 view, glm::mat4 projection);

private:
	std::unique_ptr<GEVertexBuffer> vbo;
	std::unique_ptr<GEIndexBuffer> ibo;
	std::unique_ptr<GEUniformBuffer> transformBuffer;
	std::unique_ptr<GEDescriptorSet> dset;
};

