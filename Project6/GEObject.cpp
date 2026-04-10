#include "GEObject.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

//
// FUNCIÓN: GEObject::destroy(GEGraphicsContext* gc)
//
// PROPÓSITO: Destruye los componentes del objeto
//
void GEObject::destroy(GEGraphicsContext* gc)
{
	for (auto& piece : pieces) {
		if (piece != nullptr) {
			piece->destroy(gc); // Liberamos la VRAM (Vulkan)
		}
	}
	pieces.clear();

	for (auto& tex : textures) {
		if (tex != nullptr) {
			tex->destroy(gc);
		}
	}
	textures.clear();
}


//
// FUNCIÓN: GEObject::addCommands(VkCommandBuffer commandBuffer, int index)
//
// PROPÓSITO: Añade los comandos de renderizado al command buffer
//
void GEObject::addCommands(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, int index)
{
	uint32_t count = (uint32_t)pieces.size();
	for (uint32_t i = 0; i < count; i++) pieces[i]->addCommands(commandBuffer, pipelineLayout, index);
}

//
// FUNCIÓN: GEObject::update(GEGraphicsContext* gc, uint32_t index, glm::mat4 view, glm::mat4 projection)
//
// PROPÓSITO: Actualiza las variables uniformes sobre una imagen del swapchain
//
void GEObject::update(GEGraphicsContext* gc, uint32_t index, glm::mat4 view, glm::mat4 projection)
{
	uint32_t count = (uint32_t)pieces.size();
	for (uint32_t i = 0; i < count; i++) pieces[i]->update(gc, index, view, projection, model);
}



//
// FUNCIÓN: GEObject::resetLocation()
//
// PROPÓSITO: Resetea la matriz de localización (Model).
//
void GEObject::resetLocation()
{
	model = glm::mat4(1.0f);
}

//
// FUNCIÓN: GEObject::setLocation()
//
// PROPÓSITO: Resetea la matriz de localización (Model).
//
void GEObject::setLocation(glm::mat4 m)
{
	model = glm::mat4(m);
}

//
// FUNCIÓN: GEObject::translate(glm::vec3 t)
//
// PROPÓSITO: Añade un desplazamiento la matriz de localización (Model).
//
void GEObject::translate(glm::vec3 t)
{
	model = glm::translate(model, t);
}

//
// FUNCIÓN: GEObject::rotate(float angle, glm::vec3 axis)
//
// PROPÓSITO: Añade una rotación la matriz de localización (Model).
//
void GEObject::rotate(float angle, glm::vec3 axis)
{
	model = glm::rotate(model, glm::radians(angle), axis);
}

//
// FUNCIÓN: GEFigure::setLight(GELight l)
//
// PROPÓSITO: Asigna las propiedades de la luz que ilumina la figura
//
void GEObject::setLight(GELight l)
{
	uint32_t count = (uint32_t)pieces.size();
	for (uint32_t i = 0; i < count; i++) pieces[i]->setLight(l);
}



void GEObject::setTexture(std::shared_ptr<GETexture> tex)
{
	uint32_t count = (uint32_t)pieces.size();
	for (uint32_t i = 0; i < count; i++) {
		if (pieces[i] != nullptr) {
			pieces[i]->setTexture(tex);
		}
	}
}