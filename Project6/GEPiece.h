/**
 * @file GEPiece.h
 * @brief Archivo GEPiece.h
 */
#pragma once

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
// CLASE: GEPiece
//
// DESCRIPCIÓN: Clase que describe una pieza de un objeto
//
/**
 * @class GEPiece
 * @brief Class GEPiece
 */
class GEPiece
{
protected:
	std::vector<GEVertex> vertices;
	std::vector<uint32_t> indices;
	glm::mat4 location;
	GEMaterial material;
	GELight light;
	std::shared_ptr<GETexture> texture;

public:
	/**
	 * @brief Función initialize
	 */
	void initialize(GEGraphicsContext* gc, GERenderingContext* rc);
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
	void update(GEGraphicsContext* gc, uint32_t index, glm::mat4 view, glm::mat4 projection, glm::mat4 model);
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
	 * @brief Función setMaterial
	 */
	void setMaterial(GEMaterial m);
	/**
	 * @brief Función setLight
	 */
	void setLight(GELight l);
	/**
	 * @brief Función setTexture
	 */
	void setTexture(std::shared_ptr<GETexture> texture);

	void setVertices(const std::vector<GEVertex>& v) { this->vertices = v; }
	void setIndices(const std::vector<uint32_t>& i) { this->indices = i; }

private:
	std::unique_ptr<GEVertexBuffer> vbo;
	std::unique_ptr<GEIndexBuffer> ibo;
	std::unique_ptr<GEUniformBuffer> transformBuffer;
	std::unique_ptr<GEUniformBuffer> materialBuffer;
	std::unique_ptr<GEUniformBuffer> lightBuffer;
	std::unique_ptr<GEDescriptorSet> dset;
};