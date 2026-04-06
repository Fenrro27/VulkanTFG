#pragma once
#include "GEParticle.h"


#include "GEGraphicsContext.h"
#include "GERenderingContext.h"
#include "GETransform.h"
#include "GEMaterial.h"
#include "GELight.h"
#include "GEIndexBuffer.h"
#include "GEUniformBuffer.h"
#include "GEDescriptorSet.h"
#include <glm/glm.hpp>
#include <vector>
#include "GEParticleBuffer.h"
#include "GEEmitterParams.h"


//
// CLASE: GEParticlesSystem
//
// DESCRIPCIÓN: Clase que describe una estructura de particulas
//

class GEParticlesSystem
{


	protected:
		std::vector<GEParticle> particles;
		std::vector<uint16_t> indices;
		glm::mat4 location;
		GEMaterial material;
		GELight light;

		// compite shader
		GEParticleBuffer* pboA = nullptr;
		GEIndexBuffer* ibo = nullptr;
		GEUniformBuffer* transformBuffer = nullptr;
		GEUniformBuffer* materialBuffer = nullptr;
		GEUniformBuffer* lightBuffer = nullptr;
		GEDescriptorSet* dset = nullptr;

		GEParticleBuffer* pboB = nullptr; // Buffer auxiliar para la lógica ping-pong
		GEEmitterParams emitterParams;
		GEUniformBuffer* emitterParamsBuffer = nullptr;
		float lastTime = 0.0f;

	public:
		void initialize(GEGraphicsContext* gc, GERenderingContext* rc, GETexture* sceneTexture);
		void destroy(GEGraphicsContext* gc);
		void addCommands(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, int index);
		void update(GEGraphicsContext* gc, uint32_t index, glm::mat4 view, glm::mat4 projection);
		void resetLocation();
		void setLocation(glm::mat4 m);
		void translate(glm::vec3 t);
		void rotate(float angle, glm::vec3 axis);
		void setMaterial(GEMaterial m);
		void setLight(GELight l);

		// compute shader
		GEParticleBuffer* getParticlesBufferA() { return pboA; }
		GEParticleBuffer* getParticlesBufferB() { return pboB; }
		size_t getParticlesSize() { return sizeof(GEParticle) * particles.size(); }
		uint32_t getParticlesCount() { return (uint32_t)particles.size(); }
		void addParticle(GEParticle p);
		VkDescriptorSet getDescriptorSet(int index) { return dset->descriptorSets[index]; }
		GEUniformBuffer* getEmitterParamsBuffer() { return emitterParamsBuffer;  }


	};



