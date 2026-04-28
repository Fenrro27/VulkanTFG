/**

 * @file GEParticlesSystem.cpp

 * @brief Archivo GEParticlesSystem.cpp

 */

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


	emitterParams.emitterPos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f); // Origen
	emitterParams.force = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f); // Sin fuerzas externas
	emitterParams.startColor = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f); // Naranja por defecto
	emitterParams.endColor = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f); // Transparente por defecto
	emitterParams.deltaTime = 0.016f;
	emitterParams.time = 0.0f;
	emitterParams.randomness = 1.0f;
	emitterParams.startSize = 1.0f;
	emitterParams.endSize = 0.1f;
	emitterParams.particleType = 0;

}



//

// FUNCIï¿½N: GEParticlesSystem::initialize(GEGraphicsContext* gc, GERenderingContext* rc)

//

// PROPï¿½SITO: Crea la figura

//

/**

 * @brief FunciÃ³n GEParticlesSystem::initialize

 */

void GEParticlesSystem::initialize(GEGraphicsContext* gc, GERenderingContext* rc, GETexture* sceneTexture)

{

	size_t vertexSize = sizeof(GEParticle) * particles.size();

	pboA = std::make_unique<GEParticleBuffer>(gc, vertexSize, particles.data());

	pboB = std::make_unique<GEParticleBuffer>(gc, vertexSize, nullptr);



	// SOLO crear el IBO si hay ï¿½ndices, compute shader

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



	/**

	 * @brief FunciÃ³n ubos

	 */

	std::vector<GEUniformBuffer*> ubos(3);

	ubos[0] = transformBuffer.get();

	ubos[1] = materialBuffer.get();

	ubos[2] = lightBuffer.get();



//	std::cout << "DescriptorSet: " << ubos.size() << " buffers, texture: " << (sceneTexture != nullptr) << std::endl;

	/**

	 * @brief FunciÃ³n tex

	 */

	std::vector<GETexture*> tex(1);

	tex[0] = sceneTexture;



	dset = std::make_unique<GEDescriptorSet>(gc, rc, ubos, tex);



	location = glm::mat4(1.0f);



	emitterParamsBuffer = std::make_unique < GEUniformBuffer>(gc, rc->imageCount, sizeof(GEEmitterParams));



}



//

// FUNCIï¿½N: GEFigure::destroy(GEGraphicsContext* gc)

//

// PROPï¿½SITO: Libera los buffers de la figura

//

/**

 * @brief FunciÃ³n GEParticlesSystem::destroy

 */

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

// FUNCIï¿½N: CAFigure::addCommands(VkCommandBuffer commandBuffer, int index)

//

// PROPï¿½SITO: Aï¿½ade los comandos de renderizado al command buffer

//

/**

 * @brief FunciÃ³n GEParticlesSystem::addCommands

 */

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

// FUNCIï¿½N: GEFigure::update(GEGraphicsContext* gc, uint32_t index, glm::mat4 view, glm::mat4 projection)

//

// PROPï¿½SITO: Actualiza las variables uniformes sobre una imagen del swapchain

//

/**

 * @brief FunciÃ³n GEParticlesSystem::update

 */

void GEParticlesSystem::update(GEGraphicsContext* gc, uint32_t index, glm::mat4 view, glm::mat4 projection)

{

	// ActualizaciÃ³n de transformaciones (Visual)

	GETransform transform;

	transform.MVP = projection * view * location;

	transform.ModelViewMatrix = view * location;

	transform.ViewMatrix = view;



	// ActualizaciÃ³n de Buffers de materiales y luces

	transformBuffer->update(gc, index, sizeof(GETransform), &transform);

	materialBuffer->update(gc, index, sizeof(GEMaterial), &material);

	lightBuffer->update(gc, index, sizeof(GELight), &light);

}



//

// FUNCIï¿½N: GEFigure::resetLocation()

//

// PROPï¿½SITO: Resetea la matriz de localizaciï¿½n (Model).

//

/**

 * @brief FunciÃ³n GEParticlesSystem::resetLocation

 */

void GEParticlesSystem::resetLocation()

{

	location = glm::mat4(1.0f);

}



//

// FUNCIï¿½N: GEFigure::setLocation()

//

// PROPï¿½SITO: Resetea la matriz de localizaciï¿½n (Model).

//

/**

 * @brief FunciÃ³n GEParticlesSystem::setLocation

 */

void GEParticlesSystem::setLocation(glm::mat4 m)

{

	location = glm::mat4(m);

}



//

// FUNCIï¿½N: GEFigure::translate(glm::vec3 t)

//

// PROPï¿½SITO: Aï¿½ade un desplazamiento la matriz de localizaciï¿½n (Model).

//

/**

 * @brief FunciÃ³n GEParticlesSystem::translate

 */

void GEParticlesSystem::translate(glm::vec3 t)

{

	location = glm::translate(location, t);

}



//

// FUNCIï¿½N: GEFigure::rotate(float angle, glm::vec3 axis)

//

// PROPï¿½SITO: Aï¿½ade una rotaciï¿½n la matriz de localizaciï¿½n (Model).

//

/**

 * @brief FunciÃ³n GEParticlesSystem::rotate

 */

void GEParticlesSystem::rotate(float angle, glm::vec3 axis)

{

	location = glm::rotate(location, glm::radians(angle), axis);

}



//

// FUNCIï¿½N: GEFigure::setMaterial(GEMaterial m)

//

// PROPï¿½SITO: Asigna las propiedades del material del que estï¿½ formada la figura

//

/**

 * @brief FunciÃ³n GEParticlesSystem::setMaterial

 */

void GEParticlesSystem::setMaterial(GEMaterial m)

{

	this->material = m;

}



//

// FUNCIï¿½N: GEFigure::setLight(GELight l)

//

// PROPï¿½SITO: Asigna las propiedades de la luz que ilumina la figura

//

/**

 * @brief FunciÃ³n GEParticlesSystem::setLight

 */

void GEParticlesSystem::setLight(GELight l)

{

	this->light = l;

}





//

// FUNCIï¿½N: GEParticlesSystem::addParticle(GEParticle p)

//

// PROPï¿½SITO: Aï¿½ade una nueva particula a la lista del sistema antes de la inicializaciï¿½n

//

/**

 * @brief FunciÃ³n GEParticlesSystem::addParticle

 */

void GEParticlesSystem::addParticle(GEParticle p)

{

	this->particles.push_back(p);

}





// En GEParticlesSystem.cpp

/**

 * @brief FunciÃ³n GEParticlesSystem::updatePhysics

 */

void GEParticlesSystem::updatePhysics(GEGraphicsContext* gc, uint32_t index, float fixedDeltaTime) {

	// Usamos el tiempo fijo en lugar del variable calculado por glfwGetTime

	emitterParams.deltaTime = fixedDeltaTime;

	emitterParams.time += fixedDeltaTime; // Acumulamos tiempo de simulaciÃ³n



	// Subimos los parÃ¡metros actualizados a la GPU

	emitterParamsBuffer->update(gc, index, sizeof(GEEmitterParams), &emitterParams);

}