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
	GERenderingContext* rc;
	GESkybox* skybox;
	//GEObject* plane; 
	std::vector<GEFigure*> figures;
	std::vector<GETexture*> textures;
	GECamera* camera;
	glm::mat4 projection;

	//compute shader
	GEComputeShader* particleCompute;
	std::vector<GEParticlesSystem*> particleSystem;
	//std::vector<GETexture*> particleTextures;
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
	GEPipelineConfig* createSkyboxPipelineConfig(VkExtent2D extent);
	GEPipelineConfig* createScenePipelineConfig(VkExtent2D extent);
	GEPipelineConfig* createParticlePipelineConfig(VkExtent2D extent);

};

