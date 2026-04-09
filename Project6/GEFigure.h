#pragma once

#include "GEGraphicsContext.h"
#include "GERenderingContext.h"
#include "GEVertex.h"
#include "GETransform.h"
#include "GEMaterial.h"
#include "GELight.h"
#include "GEVertexBuffer.h"
#include "GEIndexBuffer.h"
#include "GEUniformBuffer.h"
#include "GEDescriptorSet.h"
#include "GETexture.h"
#include <glm/glm.hpp>
#include <vector>

//
// CLASE: GEFigure
//
// DESCRIPCIÓN: Clase que describe una figura formada por una malla de vértices
//
class GEFigure
{
protected:
	std::vector<GEVertex> vertices;
	std::vector<uint32_t> indices;
	glm::mat4 location;
	GEMaterial material;
	GELight light;
	GETexture* texture; // Gestionado externamente

public:
	void initialize(GEGraphicsContext* gc, GERenderingContext* rc);
	void destroy(GEGraphicsContext* gc);
	void addCommands(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, int index);
	void update(GEGraphicsContext* gc, uint32_t index, glm::mat4 view, glm::mat4 projection);
	void resetLocation();
	void setLocation(glm::mat4 m);
	void translate(glm::vec3 t);
	void rotate(float angle, glm::vec3 axis);
	void setMaterial(GEMaterial m);
	void setLight(GELight l);
	void setTexture(GETexture* texture);

	void scale(glm::vec3 s);

private:
	std::unique_ptr < GEVertexBuffer> vbo;
	std::unique_ptr < GEIndexBuffer> ibo;
	std::unique_ptr < GEUniformBuffer> transformBuffer;
	std::unique_ptr < GEUniformBuffer> materialBuffer;
	std::unique_ptr < GEUniformBuffer> lightBuffer;
	std::unique_ptr < GEDescriptorSet> dset;
};

