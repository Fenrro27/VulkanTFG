#include "GEParticlesSystem.h"





#include "GEParticle.h"
#include "GETransform.h"
#include "GEMaterial.h"
#include "GELight.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

GEParticlesSystem::GEParticlesSystem() {
	std::cout << "GEPS" << std::endl;
	// --- VALORES POR DEFECTO DEL EMISOR ---
	// Previene lecturas de basura (NaN) en la GPU en el frame 1

	emitterParams.emitterPos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f); // Origen
	emitterParams.force = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f); // Sin fuerzas externas

	emitterParams.startColor = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f); // Naranja por defecto
	emitterParams.endColor = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f); // Transparente por defecto

	// ¡MUY IMPORTANTE! 
	// Asignamos un deltaTime simulado inicial (~60fps) para evitar que el shader 
	// multiplique por 0.0 o por un número corrupto antes del primer update.
	emitterParams.deltaTime = 0.016f;
	emitterParams.time = 0.0f;

	emitterParams.randomness = 1.0f;
	emitterParams.startSize = 1.0f;
	emitterParams.endSize = 0.1f;
	emitterParams.particleType = 0;
}

//
// FUNCI�N: GEParticlesSystem::initialize(GEGraphicsContext* gc, GERenderingContext* rc)
//
// PROP�SITO: Crea la figura
//
void GEParticlesSystem::initialize(GEGraphicsContext* gc, GERenderingContext* rc, GETexture* sceneTexture)
{
	size_t vertexSize = sizeof(GEParticle) * particles.size();
	pboA = std::make_unique<GEParticleBuffer>(gc, vertexSize, particles.data());
	pboB = std::make_unique<GEParticleBuffer>(gc, vertexSize, nullptr);

	// SOLO crear el IBO si hay �ndices, compute shader
	if (!indices.empty()) { 
		size_t indexSize = sizeof(indices[0]) * indices.size();
		ibo = std::make_unique < GEIndexBuffer>(gc, indexSize, indices.data());
	}
	else {
		ibo = nullptr; 
	}
	size_t transformBufferSize = sizeof(GETransform);
	transformBuffer = std::make_unique < GEUniformBuffer>(gc, rc->imageCount, transformBufferSize);

	size_t materialBufferSize = sizeof(GEMaterial);
	materialBuffer = std::make_unique < GEUniformBuffer>(gc, rc->imageCount, materialBufferSize);

	size_t lightBufferSize = sizeof(GELight);
	lightBuffer = std::make_unique < GEUniformBuffer>(gc, rc->imageCount, lightBufferSize);

	std::vector<GEUniformBuffer*> ubos(3);
	ubos[0] = transformBuffer.get();
	ubos[1] = materialBuffer.get();
	ubos[2] = lightBuffer.get();

//	std::cout << "DescriptorSet: " << ubos.size() << " buffers, texture: " << (sceneTexture != nullptr) << std::endl;
	std::vector<GETexture*> tex(1);
	tex[0] = sceneTexture;

	dset = std::make_unique<GEDescriptorSet>(gc, rc, ubos, tex);

	location = glm::mat4(1.0f);

	emitterParamsBuffer = std::make_unique < GEUniformBuffer>(gc, rc->imageCount, sizeof(GEEmitterParams));

}

//
// FUNCI�N: GEFigure::destroy(GEGraphicsContext* gc)
//
// PROP�SITO: Libera los buffers de la figura
//
void GEParticlesSystem::destroy(GEGraphicsContext* gc)
{
	if (pboA) pboA->destroy(gc);
	if (pboB) pboB->destroy(gc);
	if (ibo) ibo->destroy(gc);
	if (transformBuffer) transformBuffer->destroy(gc);
	if (materialBuffer) materialBuffer->destroy(gc);
	if (lightBuffer) lightBuffer->destroy(gc);
	if (emitterParamsBuffer) emitterParamsBuffer->destroy(gc);
	if (dset) dset->destroy(gc);

	// Resetemos los unique_ptr para que ya no apunten a nada
	// Esto evita que intenten borrar de nuevo al cerrarse la app
	pboA.reset();
	pboB.reset();
	ibo.reset();
	transformBuffer.reset();
	materialBuffer.reset();
	lightBuffer.reset();
	emitterParamsBuffer.reset();
	dset.reset();


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

	if (!indices.empty() ) {
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
	// 1. Actualización de transformaciones (Visual)
	GETransform transform;
	transform.MVP = projection * view * location;
	transform.ModelViewMatrix = view * location;
	transform.ViewMatrix = view;

	// 2. Actualización de Buffers de materiales y luces
	transformBuffer->update(gc, index, sizeof(GETransform), &transform);
	materialBuffer->update(gc, index, sizeof(GEMaterial), &material);
	lightBuffer->update(gc, index, sizeof(GELight), &light);
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


// En GEParticlesSystem.cpp
void GEParticlesSystem::updatePhysics(GEGraphicsContext* gc, uint32_t index, float fixedDeltaTime) {
	// Usamos el tiempo fijo en lugar del variable calculado por glfwGetTime
	emitterParams.deltaTime = fixedDeltaTime;
	emitterParams.time += fixedDeltaTime; // Acumulamos tiempo de simulación

	// Subimos los parámetros actualizados a la GPU
	emitterParamsBuffer->update(gc, index, sizeof(GEEmitterParams), &emitterParams);
}