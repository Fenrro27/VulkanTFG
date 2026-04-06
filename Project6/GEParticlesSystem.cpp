#include "GEParticlesSystem.h"





#include "GEParticle.h"
#include "GETransform.h"
#include "GEMaterial.h"
#include "GELight.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

//
// FUNCI�N: GEParticlesSystem::initialize(GEGraphicsContext* gc, GERenderingContext* rc)
//
// PROP�SITO: Crea la figura
//
void GEParticlesSystem::initialize(GEGraphicsContext* gc, GERenderingContext* rc, GETexture* sceneTexture)
{
	size_t vertexSize = sizeof(GEParticle) * particles.size();
	pboA = new GEParticleBuffer(gc, vertexSize, particles.data());
	pboB = new GEParticleBuffer(gc, vertexSize, nullptr);

	// SOLO crear el IBO si hay �ndices, compute shader
	if (indices.size() > 0) { 
		size_t indexSize = sizeof(indices[0]) * indices.size();
		ibo = new GEIndexBuffer(gc, indexSize, indices.data());
	}
	else {
		ibo = nullptr; 
	}
	size_t transformBufferSize = sizeof(GETransform);
	transformBuffer = new GEUniformBuffer(gc, rc->imageCount, transformBufferSize);

	size_t materialBufferSize = sizeof(GEMaterial);
	materialBuffer = new GEUniformBuffer(gc, rc->imageCount, materialBufferSize);

	size_t lightBufferSize = sizeof(GELight);
	lightBuffer = new GEUniformBuffer(gc, rc->imageCount, lightBufferSize);

	std::vector<GEUniformBuffer*> ubos(3);
	ubos[0] = transformBuffer;
	ubos[1] = materialBuffer;
	ubos[2] = lightBuffer;

//	std::cout << "DescriptorSet: " << ubos.size() << " buffers, texture: " << (sceneTexture != nullptr) << std::endl;
	std::vector<GETexture*> tex(1);
	tex[0] = sceneTexture;

	dset = new GEDescriptorSet(gc, rc, ubos, tex);

	location = glm::mat4(1.0f);

	emitterParamsBuffer = new GEUniformBuffer(gc, rc->imageCount, sizeof(GEEmitterParams));

}

//
// FUNCI�N: GEFigure::destroy(GEGraphicsContext* gc)
//
// PROP�SITO: Libera los buffers de la figura
//
void GEParticlesSystem::destroy(GEGraphicsContext* gc)
{
	pboA->destroy(gc);
	transformBuffer->destroy(gc);
	materialBuffer->destroy(gc);
	lightBuffer->destroy(gc);
	dset->destroy(gc);

	delete pboA;
	delete transformBuffer;
	delete materialBuffer;
	delete lightBuffer;
	delete dset;

	if (ibo) { // compute shader
		ibo->destroy(gc);
		delete ibo;
	}

	if (emitterParamsBuffer) {
		emitterParamsBuffer->destroy(gc);
		delete emitterParamsBuffer;
	}

	if (pboB) {
		pboB->destroy(gc);
		delete pboB;
	}


}

//
// FUNCI�N: CAFigure::addCommands(VkCommandBuffer commandBuffer, int index)
//
// PROP�SITO: A�ade los comandos de renderizado al command buffer
//
void GEParticlesSystem::addCommands(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, int index)
{
	// compute shader
	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, &(pboA->buffer), &offset);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &(dset->descriptorSets[index]), 0, nullptr);

	if (indices.size() > 0) {
		vkCmdBindIndexBuffer(commandBuffer, ibo->buffer, 0, VK_INDEX_TYPE_UINT16);
		vkCmdDrawIndexed(commandBuffer, (uint32_t)indices.size(), 1, 0, 0, 0);
	}
	else {
		vkCmdDraw(commandBuffer, (uint32_t)particles.size(), 1, 0, 0);
	}

}

//
// FUNCI�N: GEFigure::update(GEGraphicsContext* gc, uint32_t index, glm::mat4 view, glm::mat4 projection)
//
// PROP�SITO: Actualiza las variables uniformes sobre una imagen del swapchain
//
void GEParticlesSystem::update(GEGraphicsContext* gc, uint32_t index, glm::mat4 view, glm::mat4 projection)
{
	GETransform transform;
	transform.MVP = projection * view * location;
	transform.ModelViewMatrix = view * location;
	transform.ViewMatrix = view;

	transformBuffer->update(gc, index, sizeof(GETransform), &transform);
	materialBuffer->update(gc, index, sizeof(GEMaterial), &material);
	lightBuffer->update(gc, index, sizeof(GELight), &light);

	// Calcular el deltaTime real usando GLFW
	float currentTime = (float)glfwGetTime();
	if (lastTime == 0.0f) lastTime = currentTime; // Evitar salto en el primer frame

	emitterParams.deltaTime = currentTime - lastTime;
	emitterParams.time = currentTime;
	lastTime = currentTime;

	// Enviar a la GPU
	emitterParamsBuffer->update(gc, index, sizeof(GEEmitterParams), &emitterParams);
}

//
// FUNCI�N: GEFigure::resetLocation()
//
// PROP�SITO: Resetea la matriz de localizaci�n (Model).
//
void GEParticlesSystem::resetLocation()
{
	location = glm::mat4(1.0f);
}

//
// FUNCI�N: GEFigure::setLocation()
//
// PROP�SITO: Resetea la matriz de localizaci�n (Model).
//
void GEParticlesSystem::setLocation(glm::mat4 m)
{
	location = glm::mat4(m);
}

//
// FUNCI�N: GEFigure::translate(glm::vec3 t)
//
// PROP�SITO: A�ade un desplazamiento la matriz de localizaci�n (Model).
//
void GEParticlesSystem::translate(glm::vec3 t)
{
	location = glm::translate(location, t);
}

//
// FUNCI�N: GEFigure::rotate(float angle, glm::vec3 axis)
//
// PROP�SITO: A�ade una rotaci�n la matriz de localizaci�n (Model).
//
void GEParticlesSystem::rotate(float angle, glm::vec3 axis)
{
	location = glm::rotate(location, glm::radians(angle), axis);
}

//
// FUNCI�N: GEFigure::setMaterial(GEMaterial m)
//
// PROP�SITO: Asigna las propiedades del material del que est� formada la figura
//
void GEParticlesSystem::setMaterial(GEMaterial m)
{
	this->material = m;
}

//
// FUNCI�N: GEFigure::setLight(GELight l)
//
// PROP�SITO: Asigna las propiedades de la luz que ilumina la figura
//
void GEParticlesSystem::setLight(GELight l)
{
	this->light = l;
}


//
// FUNCI�N: GEParticlesSystem::addParticle(GEParticle p)
//
// PROP�SITO: A�ade una nueva particula a la lista del sistema antes de la inicializaci�n
//
void GEParticlesSystem::addParticle(GEParticle p)
{
	this->particles.push_back(p);
}