/**
 * @file GEObject.h
 * @brief Archivo GEObject.h
 */
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
/**
 * @class GEObject
 * @brief Class GEObject
 */
class GEObject
{
protected:
	std::vector<GEMaterial> materials;
	std::vector<std::shared_ptr<GETexture>> textures;
	std::vector<std::unique_ptr<GEPiece>> pieces;
	glm::mat4 model = glm::mat4(1.0f);  // Model matrix

public:
	/**
	 * @brief Función destroy
	 */
	void destroy(GEGraphicsContext* gc);
	/**
	 * @brief Función addCommands
	 */
	void addCommands(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, int index);
	/**
	 * @brief Función update
	 */
	void update(GEGraphicsContext* gc, uint32_t index, glm::mat4 view, glm::mat4 projection);
	/**
	 * @brief Función resetLocation
	 */
	void resetLocation();
	/**
	 * @brief Función setLocation
	 */
	void setLocation(glm::mat4 m);
	/**
	 * @brief Función translate
	 */
	void translate(glm::vec3 t);
	/**
	 * @brief Función rotate
	 */
	void rotate(float angle, glm::vec3 axis);
	/**
	 * @brief Función setLight
	 */
	void setLight(GELight l);
	/**
	 * @brief Función setTexture
	 */
	void setTexture(std::shared_ptr<GETexture> tex);
};

