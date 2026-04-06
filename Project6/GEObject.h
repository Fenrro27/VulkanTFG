#pragma once

#include "GEGraphicsContext.h"
#include "GERenderingContext.h"
#include "GELight.h"
#include "GEPiece.h"
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

//
// CLASE: GEObject
//
// DESCRIPCIÓN: Clase abstracta que representa un objeto formado por varias piezas
// 
class GEObject
{
protected:
	std::vector<GEMaterial> materials;
	std::vector<GETexture*> textures;
	std::vector<GEPiece*> pieces;
	glm::mat4 model;  // Model matrix

public:
	void destroy(GEGraphicsContext* gc);
	void addCommands(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, int index);
	void update(GEGraphicsContext* gc, uint32_t index, glm::mat4 view, glm::mat4 projection);
	void resetLocation();
	void setLocation(glm::mat4 m);
	void translate(glm::vec3 t);
	void rotate(float angle, glm::vec3 axis);
	void setLight(GELight l);
};

