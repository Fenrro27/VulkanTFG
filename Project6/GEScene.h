#pragma once

#include "GEGraphicsContext.h"
#include "GEDrawingContext.h"
#include "GECommandContext.h"
#include "GERenderingContext.h"

#include "GESkybox.h"
#include "GEObject.h"
#include "GEFigure.h"
#include "GEParticlesSystem.h"
#include "GECamera.h"
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <vector>
#include "GEComputeShader.h"



#define SKYBOX_PIPELINE 0
#define SCENE_PIPELINE 1
#define PARTICLE_PIPELINE 2

//
// CLASE: GEScene
//
// DESCRIPCIÓN: Clase que describe una escena
//
class GEScene
{
private:
	std::unique_ptr<GERenderingContext> rc;
	std::unique_ptr<GESkybox> skybox;
	std::unique_ptr<GECamera> camera;
	std::unique_ptr<GEComputeShader> particleCompute;

	//GEObject* plane; 
	std::vector<std::unique_ptr<GEFigure>> figures;
	std::vector<std::unique_ptr<GEObject>> objects;
	std::vector< std::unique_ptr<GEParticlesSystem>> particleSystem;
	
	std::vector< std::shared_ptr<GETexture>> textures;

	glm::mat4 projection;
	uint32_t particleCount = 10000;



public:
	GEScene(GEGraphicsContext* gc, GEDrawingContext* dc, GECommandContext* cc);
	void destroy(GEGraphicsContext* gc);
	void recreate(GEGraphicsContext* gc, GEDrawingContext* dc, GECommandContext* cc);
	void update(GEGraphicsContext* gc, uint32_t index);
	void key_action(int key, bool pressed);
	void aspect_ratio(double aspect);

private:
	void fillCommandBuffers(GECommandContext* commandBuffers);
	std::unique_ptr < GEPipelineConfig> createSkyboxPipelineConfig(VkExtent2D extent);
	std::unique_ptr < GEPipelineConfig> createScenePipelineConfig(VkExtent2D extent);
	std::unique_ptr < GEPipelineConfig> createParticlePipelineConfig(VkExtent2D extent);

};

