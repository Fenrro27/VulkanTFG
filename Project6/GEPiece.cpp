/**
 * @file GEPiece.cpp
 * @brief Archivo GEPiece.cpp
 */
#include "GEPiece.h"

#include "GEVertex.h"
#include "GETransform.h"
#include "GEMaterial.h"
#include "GELight.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

//
// FUNCIÓN: GEPiece::initialize(GEGraphicsContext* gc, GERenderingContext* rc)
//
// PROPÓSITO: Crea la pieza
//
/**
 * @brief Función GEPiece::initialize
 */
void GEPiece::initialize(GEGraphicsContext* gc, GERenderingContext* rc)
{
	if (this->texture == nullptr) {
		std::cout << "[DEBUG_WARN] GEPiece sin textura detectada. Cargando wood.jpg por defecto." << std::endl;
		this->texture = std::shared_ptr<GETexture>(
			/**
			 * @brief Función GETexture
			 */
			new GETexture(gc, "textures/wood.jpg"),
			[gc](GETexture* t) { t->destroy(gc); }
		);
	}

	size_t vertexSize = sizeof(GEVertex) * vertices.size();
	vbo = std::make_unique <GEVertexBuffer>(gc, vertexSize, vertices.data());

	size_t indexSize = sizeof(indices[0]) * indices.size();
	ibo = std::make_unique < GEIndexBuffer>(gc, indexSize, indices.data());

	size_t transformBufferSize = sizeof(GETransform);
	transformBuffer = std::make_unique < GEUniformBuffer>(gc, rc->imageCount, transformBufferSize);

	size_t materialBufferSize = sizeof(GEMaterial);
	materialBuffer = std::make_unique < GEUniformBuffer>(gc, rc->imageCount, materialBufferSize);

	size_t lightBufferSize = sizeof(GELight);
	lightBuffer = std::make_unique < GEUniformBuffer>(gc, rc->imageCount, lightBufferSize);

	/**
	 * @brief Función ubos
	 */
	std::vector<GEUniformBuffer*> ubos(3);
	ubos[0] = transformBuffer.get();
	ubos[1] = materialBuffer.get();
	ubos[2] = lightBuffer.get();

	/**
	 * @brief Función tex
	 */
	std::vector<GETexture*> tex(1);
	tex[0] = texture.get();

	dset = std::make_unique<GEDescriptorSet>(gc, rc, ubos, tex);

	location = glm::mat4(1.0f);
}

//
// FUNCIÓN: GEPiece::destroy(GEGraphicsContext* gc)
//
// PROPÓSITO: Libera los buffers de la pieza
//
/**
 * @brief Función GEPiece::destroy
 */
void GEPiece::destroy(GEGraphicsContext* gc)
{
	vbo->destroy(gc);
	ibo->destroy(gc);
	transformBuffer->destroy(gc);
	materialBuffer->destroy(gc);
	lightBuffer->destroy(gc);
	dset->destroy(gc);
}

//
// FUNCIÓN: GEPiece::addCommands(VkCommandBuffer commandBuffer, int index)
//
// PROPÓSITO: Añade los comandos de renderizado al command buffer
//
/**
 * @brief Función GEPiece::addCommands
 */
void GEPiece::addCommands(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, int index)
{
	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &(vbo->buffer), &offset);
	vkCmdBindIndexBuffer(commandBuffer, ibo->buffer, 0, VK_INDEX_TYPE_UINT32);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &(dset->descriptorSets[index]), 0, nullptr);
	vkCmdDrawIndexed(commandBuffer, (uint32_t)indices.size(), 1, 0, 0, 0);
}

//
// FUNCIÓN: GEPiece::update(GEGraphicsContext* gc, uint32_t index, glm::mat4 view, glm::mat4 projection)
//
// PROPÓSITO: Actualiza las variables uniformes sobre una imagen del swapchain
//
/**
 * @brief Función GEPiece::update
 */
void GEPiece::update(GEGraphicsContext* gc, uint32_t index, glm::mat4 view, glm::mat4 projection, glm::mat4 model)
{
	GETransform transform;
	transform.MVP = projection * view * model * location;
	transform.ModelViewMatrix = view * model * location;
	transform.ViewMatrix = view;

	transformBuffer->update(gc, index, sizeof(GETransform), &transform);
	materialBuffer->update(gc, index, sizeof(GEMaterial), &material);
	lightBuffer->update(gc, index, sizeof(GELight), &light);
}

//
// FUNCIÓN: GEPiece::resetLocation()
//
// PROPÓSITO: Resetea la matriz de localización (Model).
//
/**
 * @brief Función GEPiece::resetLocation
 */
void GEPiece::resetLocation()
{
	location = glm::mat4(1.0f);
}

//
// FUNCIÓN: GEPiece::setLocation()
//
// PROPÓSITO: Resetea la matriz de localización (Model).
//
/**
 * @brief Función GEPiece::setLocation
 */
void GEPiece::setLocation(glm::mat4 m)
{
	location = glm::mat4(m);
}

//
// FUNCIÓN: GEPiece::translate(glm::vec3 t)
//
// PROPÓSITO: Añade un desplazamiento la matriz de localización (Model).
//
/**
 * @brief Función GEPiece::translate
 */
void GEPiece::translate(glm::vec3 t)
{
	location = glm::translate(location, t);
}

//
// FUNCIÓN: GEPiece::rotate(float angle, glm::vec3 axis)
//
// PROPÓSITO: Añade una rotación la matriz de localización (Model).
//
/**
 * @brief Función GEPiece::rotate
 */
void GEPiece::rotate(float angle, glm::vec3 axis)
{
	location = glm::rotate(location, glm::radians(angle), axis);
}

//
// FUNCIÓN: GEPiece::setMaterial(GEMaterial m)
//
// PROPÓSITO: Asigna las propiedades del material del que está formada la pieza
//
/**
 * @brief Función GEPiece::setMaterial
 */
void GEPiece::setMaterial(GEMaterial m)
{
	this->material = m;
}

//
// FUNCIÓN: GEPiece::setLight(GELight l)
//
// PROPÓSITO: Asigna las propiedades de la luz que ilumina la pieza
//
/**
 * @brief Función GEPiece::setLight
 */
void GEPiece::setLight(GELight l)
{
	this->light = l;
}

//
// FUNCIÓN: GEPiece::setTexture(GETexture* tex)
//
// PROPÓSITO: Asigna la textura
//
/**
 * @brief Función GEPiece::setTexture
 */
void GEPiece::setTexture(std::shared_ptr<GETexture> tex)
{
	this->texture = tex;
}
