/**
 * @file GEFigure.cpp
 * @brief Archivo GEFigure.cpp
 */
#include "GEFigure.h"

#include "GEVertex.h"
#include "GETransform.h"
#include "GEMaterial.h"
#include "GELight.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "commonDebug.h"

//
// FUNCIÃN: GEFigure::initialize(GEGraphicsContext* gc, GERenderingContext* rc)
//
// PROPÃSITO: Crea la figura
//
/**
 * @brief FunciÃ³n GEFigure::initialize
 */
void GEFigure::initialize(GEGraphicsContext* gc, GERenderingContext* rc)
{
	GE_DEBUG_INFO("Initializing figure");

	if (texture == nullptr) {
		texture = std::make_unique<GETexture>(gc, "textures/wood.jpg").get();
	}
	else {
		std::cout << "Texture already set for this figure" << std::endl;
	}

	size_t vertexSize = sizeof(GEVertex) * vertices.size();
	vbo = std::make_unique<GEVertexBuffer>(gc, vertexSize, vertices.data());

	size_t indexSize = sizeof(indices[0]) * indices.size();
	ibo = std::make_unique < GEIndexBuffer>(gc, indexSize, indices.data());

	size_t transformBufferSize = sizeof(GETransform);
	transformBuffer = std::make_unique < GEUniformBuffer>(gc, rc->imageCount, transformBufferSize);

	size_t materialBufferSize = sizeof(GEMaterial);
	materialBuffer = std::make_unique < GEUniformBuffer>(gc, rc->imageCount, materialBufferSize);

	size_t lightBufferSize = sizeof(GELight);
	lightBuffer = std::make_unique < GEUniformBuffer>(gc, rc->imageCount, lightBufferSize);

	/**
	 * @brief FunciÃ³n ubos
	 */
	std::vector<GEUniformBuffer*> ubos(3);
	ubos[0] = transformBuffer.get();
	ubos[1] = materialBuffer.get();
	ubos[2] = lightBuffer.get();

	/**
	 * @brief FunciÃ³n tex
	 */
	std::vector<GETexture*> tex(1);
	tex[0] = texture;

	dset = std::make_unique<GEDescriptorSet>(gc, rc, ubos, tex);

	location = glm::mat4(1.0f);
}

//
// FUNCIÃN: GEFigure::destroy(GEGraphicsContext* gc)
//
// PROPÃSITO: Libera los buffers de la figura
//
/**
 * @brief FunciÃ³n GEFigure::destroy
 */
void GEFigure::destroy(GEGraphicsContext* gc)
{
	GE_DEBUG_ERROR("Destroying figure");
	vbo->destroy(gc);
	ibo->destroy(gc);
	transformBuffer->destroy(gc);
	materialBuffer->destroy(gc);
	lightBuffer->destroy(gc);
	dset->destroy(gc);

	//if (texture != nullptr) {
	//	texture->destroy(gc);
	//}
	
}

//
// FUNCIÃN: CAFigure::addCommands(VkCommandBuffer commandBuffer, int index)
//
// PROPÃSITO: AÃ±ade los comandos de renderizado al command buffer
//
/**
 * @brief FunciÃ³n GEFigure::addCommands
 */
void GEFigure::addCommands(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, int index)
{
	
	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &(vbo->buffer), &offset);
	vkCmdBindIndexBuffer(commandBuffer, ibo->buffer, 0, VK_INDEX_TYPE_UINT32);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &(dset->descriptorSets[index]), 0, nullptr);
	vkCmdDrawIndexed(commandBuffer, (uint32_t)indices.size(), 1, 0, 0, 0);
}

//
// FUNCIÃN: GEFigure::update(GEGraphicsContext* gc, uint32_t index, glm::mat4 view, glm::mat4 projection)
//
// PROPÃSITO: Actualiza las variables uniformes sobre una imagen del swapchain
//
/**
 * @brief FunciÃ³n GEFigure::update
 */
void GEFigure::update(GEGraphicsContext* gc, uint32_t index, glm::mat4 view, glm::mat4 projection)
{
	GETransform transform;
	transform.MVP = projection * view * location;
	transform.ModelViewMatrix = view * location;
	transform.ViewMatrix = view;

	transformBuffer->update(gc, index, sizeof(GETransform), &transform);
	materialBuffer->update(gc, index, sizeof(GEMaterial), &material);
	lightBuffer->update(gc, index, sizeof(GELight), &light);
}

//
// FUNCIÃN: GEFigure::resetLocation()
//
// PROPÃSITO: Resetea la matriz de localizaciÃ³n (Model).
//
/**
 * @brief FunciÃ³n GEFigure::resetLocation
 */
void GEFigure::resetLocation()
{
	location = glm::mat4(1.0f);
}

//
// FUNCIÃN: GEFigure::setLocation()
//
// PROPÃSITO: Resetea la matriz de localizaciÃ³n (Model).
//
/**
 * @brief FunciÃ³n GEFigure::setLocation
 */
void GEFigure::setLocation(glm::mat4 m)
{
	location = glm::mat4(m);
}

//
// FUNCIÃN: GEFigure::translate(glm::vec3 t)
//
// PROPÃSITO: AÃ±ade un desplazamiento la matriz de localizaciÃ³n (Model).
//
/**
 * @brief FunciÃ³n GEFigure::translate
 */
void GEFigure::translate(glm::vec3 t)
{
	location = glm::translate(location, t);
}

//
// FUNCIÃN: GEFigure::rotate(float angle, glm::vec3 axis)
//
// PROPÃSITO: AÃ±ade una rotaciÃ³n la matriz de localizaciÃ³n (Model).
//
/**
 * @brief FunciÃ³n GEFigure::rotate
 */
void GEFigure::rotate(float angle, glm::vec3 axis)
{
	location = glm::rotate(location, glm::radians(angle), axis);
}

//
// FUNCIÃN: GEFigure::setMaterial(GEMaterial m)
//
// PROPÃSITO: Asigna las propiedades del material del que estÃ¡ formada la figura
//
/**
 * @brief FunciÃ³n GEFigure::setMaterial
 */
void GEFigure::setMaterial(GEMaterial m)
{
	this->material = m;
}

//
// FUNCIÃN: GEFigure::setLight(GELight l)
//
// PROPÃSITO: Asigna las propiedades de la luz que ilumina la figura
//
/**
 * @brief FunciÃ³n GEFigure::setLight
 */
void GEFigure::setLight(GELight l)
{
	this->light = l;
}

//
// FUNCIÃN: GEFigure::setTexture(GETexture* tex)
//
// PROPÃSITO: Asigna la textura
//
/**
 * @brief FunciÃ³n GEFigure::setTexture
 */
void GEFigure::setTexture(GETexture* tex)
{
	this->texture = tex;
}

/**
 * @brief FunciÃ³n GEFigure::scale
 */
void GEFigure::scale(glm::vec3 s)
{
	location = glm::scale(location, s);
}