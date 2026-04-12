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



enum PipelineType {
	SKYBOX_PIPELINE = 0,
	SCENE_PIPELINE = 1,
	PARTICLE_PIPELINE = 2
};

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
	void fillCommandBuffers(GECommandContext* commandBuffers);


	bool isDragging = false;
	bool firstMouse = true;
	float lastX = 400.0f; // Mitad de pantalla 800x600
	float lastY = 300.0f;


public:
	GEScene(GEGraphicsContext* gc, GEDrawingContext* dc, GECommandContext* cc);
	void destroy(GEGraphicsContext* gc);
	void recreate(GEGraphicsContext* gc, GEDrawingContext* dc, GECommandContext* cc);
	void update(GEGraphicsContext* gc, uint32_t index);
	void key_action(int key, bool pressed);
	void aspect_ratio(double aspect);

	void mouse_action(double xpos, double ypos);
	void toggle_camera_mode();
	void mouse_button_action(GLFWwindow* window, int button, int action);

	GERenderingContext* getRenderingContext() { return rc.get(); }

private:
	std::unique_ptr < GEPipelineConfig> createSkyboxPipelineConfig(VkExtent2D extent);
	std::unique_ptr < GEPipelineConfig> createScenePipelineConfig(VkExtent2D extent);
	std::unique_ptr < GEPipelineConfig> createParticlePipelineConfig(VkExtent2D extent);

};

