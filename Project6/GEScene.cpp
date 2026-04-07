#include "GEScene.h"

#include "GECube.h"
#include "GEPyramid.h"
#include "GECone.h"
#include "GECylinder.h"
#include "GESphere.h"
#include "GEDisk.h"
#include "GETorus.h"
#include "GEIcosahedron.h"
#include "GEGround.h"
#include "GETransform.h"
#include "GEMaterial.h"
#include "GELight.h"
#include <windows.h>
#include "resource.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "GEHumo.h"
#include "GEFuego.h"
#include "GEAgua.h"
#include "GETexture.h"

//
// FUNCI�N: GEScene::GEScene(GEGraphicsContext* gc, GEDrawingContext* dc)
//
// PROP�SITO: Crea la escena
//
GEScene::GEScene(GEGraphicsContext* gc, GEDrawingContext* dc, GECommandContext* cc)
{
	VkExtent2D extent = dc->getExtent();
	double aspect = (double)extent.width / (double)extent.height;
	aspect_ratio(aspect);

	auto skybox_config = createSkyboxPipelineConfig(dc->getExtent());
	rc = std::make_unique<GERenderingContext>(gc, dc, skybox_config.get());

	auto scene_config = createScenePipelineConfig(dc->getExtent());
	rc->addGraphicsPipeline(gc, scene_config.get());

	auto particle_Config = createParticlePipelineConfig(dc->getExtent());
	rc->addGraphicsPipeline(gc, particle_Config.get());

	this->camera = std::make_unique <GECamera>();
	this->camera->setPosition(glm::vec3(0.0f, 10.0f, 300.0f));
	this->camera->setMoveStep(0.0f);

	rc->setActivePipeline(SKYBOX_PIPELINE);
	this->skybox = std::make_unique<GESkybox>(gc, rc.get());

	rc->setActivePipeline(SCENE_PIPELINE);

	textures.resize(3);
	textures[0] = std::make_unique<GETexture>(gc, "textures/stone.jpg");
	textures[1] = std::make_unique<GETexture>(gc, "textures/moon.jpg");
	textures[2] = std::make_unique<GETexture>(gc, "textures/wood.jpg");

	GELight light = {};
	light.Ldir = glm::normalize(glm::vec3(1.0f, -0.8f, -0.7f));
	light.La = glm::vec3(0.2f, 0.2f, 0.2f);
	light.Ld = glm::vec3(0.8f, 0.8f, 0.8f);
	light.Ls = glm::vec3(1.0f, 1.0f, 1.0f);

	GEMaterial groundMat = {};
	groundMat.Ka = glm::vec3(1.0f, 1.0f, 1.0f);
	groundMat.Kd = glm::vec3(1.0f, 1.0f, 1.0f);
	groundMat.Ks = glm::vec3(0.8f, 0.8f, 0.8f);
	groundMat.Shininess = 16.0f;


	std::unique_ptr<GEFigure> ground = std::make_unique<GEGround>(50.0f, 50.0f);
	ground->setTexture(textures[2].get()); // Obtiene el puntero crudo del unique_ptr
	ground->initialize(gc, rc.get());
	ground->setMaterial(groundMat);
	ground->setLight(light);


	figures.push_back(std::move(ground));

	
	rc->setActivePipeline(PARTICLE_PIPELINE);

	particleCompute = std::make_unique<GEComputeShader>(gc, IDR_COMPUTE_PARTICLES, dc->getImageCount());
	
	particleSystem.push_back(std::make_unique<GEHumo>(100));
	particleSystem.push_back(std::make_unique<GEFuego>(50));
	particleSystem.push_back(std::make_unique<GEAgua>(1000));

	
	GEMaterial particle1Mat = {};
	particle1Mat.Ka = glm::vec3(0.1f, 0.1f, 0.1f);
	particle1Mat.Kd = glm::vec3(0.4f, 0.42f, 0.45f);
	particle1Mat.Ks = glm::vec3(0.05f, 0.05f, 0.05f);
	particle1Mat.Shininess = 2.0f;

	particleSystem[0]->initialize(gc, rc.get(), new GETexture(gc, "textures/smoke.png"));
	particleSystem[0]->translate(glm::vec3(-25.0f, 0.0f, 0.0f));
	particleSystem[0]->setMaterial(particle1Mat);
	particleSystem[0]->setLight(light);
	particleCompute->addParticleSystem(gc, dc->getImageCount(), particleSystem[0].get());

	particleSystem[1]->initialize(gc, rc.get(), new GETexture(gc, "textures/fire.png"));
	particleSystem[1]->translate(glm::vec3(0.0f, 0.0f, 0.0f));
	particleSystem[1]->setMaterial(particle1Mat);
	particleSystem[1]->setLight(light);
	particleCompute->addParticleSystem(gc, dc->getImageCount(), particleSystem[1].get());

	particleSystem[2]->initialize(gc, rc.get(), new GETexture(gc, "textures/pngwing.com (1).png"));
	particleSystem[2]->translate(glm::vec3(25.0f, 0.0f, 0.0f));
	particleSystem[2]->setMaterial(particle1Mat);
	particleSystem[2]->setLight(light);
	particleCompute->addParticleSystem(gc, dc->getImageCount(), particleSystem[2].get());



	fillCommandBuffers(cc);
}

//
// FUNCI�N: GEScene::destroy(GEGraphicsContext* gc)
//
// PROP�SITO: Reconstruye los componentes gr�ficos de la escena
//
void GEScene::destroy(GEGraphicsContext* gc)
{
	particleCompute->destroy(gc);
	rc->destroy(gc);
	skybox->destroy(gc);
	for (auto& figure: figures)
	{
		figure->destroy(gc);
	}
	for (auto& ps: particleSystem) {
		ps->destroy(gc);
	}
	particleSystem.clear();

}

//
// FUNCI�N: GEScene::recreate(GEGraphicsContext* gc, GEDrawingContext* dc)
//
// PROP�SITO: Reconstruye los componentes gr�ficos de la escena
//
void GEScene::recreate(GEGraphicsContext* gc, GEDrawingContext* dc, GECommandContext* cc)
{
	rc->destroy(gc);
	auto skybox_config = createSkyboxPipelineConfig(dc->getExtent());
	this->rc = std::make_unique<GERenderingContext>(gc, dc, skybox_config.get());

	auto scene_config = createScenePipelineConfig(dc->getExtent());
	this->rc->addGraphicsPipeline(gc, scene_config.get());

	auto particle_Config = createParticlePipelineConfig(dc->getExtent());
	this->rc->addGraphicsPipeline(gc, particle_Config.get());

	fillCommandBuffers(cc);
}

//
// FUNCI�N: GEScene::update(GEGraphicsContext* gc, uint32_t index)
//
// PROP�SITO: Actualiza la informaci�n para generar la imagen 
//
void GEScene::update(GEGraphicsContext* gc, uint32_t index)
{
	camera->update();
	glm::mat4 view = camera->getViewMatrix();

	skybox->update(gc, index, view, projection);

	for (auto& figure:figures)
	{
		figure->update(gc, index, view, projection);
	}

	for (auto& ps: particleSystem)
	{
		ps->update(gc, index, view, projection);
	}
	//plane->update(gc, index, view, projection);


}

//
// FUNCI�N: GEScene::key_action(int key, bool pressed)
//
// PROP�SITO: Respuesta a acciones de teclado
//
void GEScene::key_action(int key, bool pressed)
{
	switch (key)
	{
	case GLFW_KEY_UP:
		camera->setTurnDown(pressed);
		break;
	case GLFW_KEY_DOWN:
		camera->setTurnUp(pressed);
		break;
	case GLFW_KEY_LEFT:
		camera->setTurnCCW(pressed);
		break;
	case GLFW_KEY_RIGHT:
		camera->setTurnCW(pressed);
		break;
	case GLFW_KEY_S:
		camera->setMoveStep(0.0f);
		break;
	case GLFW_KEY_KP_ADD:
	case GLFW_KEY_1:
		camera->setMoveStep(camera->getMoveStep() + 0.1f);
		break;
	case GLFW_KEY_KP_SUBTRACT:
	case GLFW_KEY_2:
		camera->setMoveStep(camera->getMoveStep() - 0.1f);
		break;
	case GLFW_KEY_Q:
		camera->setMoveUp(pressed);
		break;
	case GLFW_KEY_A:
		camera->setMoveDown(pressed);
		break;
	case GLFW_KEY_O:
		camera->setMoveLeft(pressed);
		break;
	case GLFW_KEY_P:
		camera->setMoveRight(pressed);
		break;
	case GLFW_KEY_K:
		camera->setTurnLeft(pressed);
		break;
	case GLFW_KEY_L:
		camera->setTurnRight(pressed);
		break;
	}
}

//
// FUNCI�N: GEScene::aspect_ratio(double)
//
// PROP�SITO: Modifica la relaci�n anchura/altura del modelo
//
void GEScene::aspect_ratio(double aspect)
{
	constexpr double fov = glm::radians(30.0f);
	// double sin_fov = sin(fov);
	// double cos_fov = cos(fov);
	// float wHeight = (sin_fov * 0.2 / cos_fov);
	// float wWidth = (wHeight * aspect);

	projection = glm::perspective((float)fov, (float)aspect, 0.2f, 400.0f);
	projection[1][1] *= -1.0f;
}

void GEScene::fillCommandBuffers(GECommandContext* cc)
{
	cc->beginCommandBuffers();

	for (int i = 0; i < (int)cc->commandBuffers.size(); i++)
		{
			VkCommandBuffer cb = cc->commandBuffers[i];

				// --- 1. FASE DE CÓMPUTO (PARTÍCULAS) ---
				for (size_t s = 0; s < particleSystem.size(); s++)
				{
						auto& ps = particleSystem[s]; 
						uint32_t particleCount = ps->getParticlesCount(); 
						uint32_t groupCount = (particleCount + 255) / 256; 

						particleCompute->recordCommands(cb, (uint32_t)s, (uint32_t)i, groupCount);

						VkBufferMemoryBarrier barriers[2] = {};
					barriers[0].sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER; 
						barriers[0].srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT; 
						barriers[0].dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
						barriers[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED; 
						barriers[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED; 
						barriers[0].buffer = ps->getParticlesBufferA()->buffer; 
						barriers[0].offset = 0;
						barriers[0].size = VK_WHOLE_SIZE; 

						barriers[1] = barriers[0]; 
						barriers[1].buffer = ps->getParticlesBufferB()->buffer; 

						vkCmdPipelineBarrier(
							cb,
							VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
							VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
							0, 0, nullptr, 2, barriers, 0, nullptr
						); 
				}

				rc->insertBeginCommands(cb, i);

			// --- 2. DIBUJO DEL SKYBOX ---
			 	rc->setActivePipeline(SKYBOX_PIPELINE);
				// Vinculamos la pipeline SOLO al buffer actual 'cb'
				vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, rc->getActivePipeline());
				skybox->addCommands(cb, rc->getActivePipelineLayout(), i); 

				// --- 3. DIBUJO DE LA ESCENA (FIGURAS Y AVIÓN) ---
				rc->setActivePipeline(SCENE_PIPELINE);
				// Vinculamos la pipeline SOLO al buffer actual 'cb'
				vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, rc->getActivePipeline()); 

				for ( auto& figure:figures)
				{
					figure->addCommands(cb, rc->getActivePipelineLayout(), i);
				}
		//	plane->addCommands(cb, rc->getActivePipelineLayout(), i);

				// --- 4. DIBUJO DE PARTÍCULAS ---
				rc->setActivePipeline(PARTICLE_PIPELINE);
				// Vinculamos la pipeline SOLO al buffer actual 'cb'
				vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, rc->getActivePipeline()); 

				for (auto& ps:particleSystem)
				{
						uint32_t particleCount = ps->getParticlesCount(); 

						VkDescriptorSet ds = ps->getDescriptorSet(i);
						vkCmdBindDescriptorSets(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, rc->getActivePipelineLayout(), 0, 1, &ds, 0, nullptr); 

						VkDeviceSize offset = 0; 
						VkBuffer bufferADibujar = (i % 2 == 0) ? ps->getParticlesBufferB()->buffer : ps->getParticlesBufferA()->buffer; 

						vkCmdBindVertexBuffers(cb, 0, 1, &bufferADibujar, &offset); 
						vkCmdDraw(cb, particleCount, 1, 0, 0); 
				}
				rc->insertEndCommands(cb);
	}

		cc->endCommandBuffers(); 
}
	//
	// FUNCIÓN: GEScene::createSkyboxPipelineConfig()
	//
	// PROPÓSITO: Obtiene la configuración del pipeline de renderizado para el skybox
	//
std::unique_ptr <GEPipelineConfig> GEScene::createSkyboxPipelineConfig(VkExtent2D extent)
	{
		std::unique_ptr<GEPipelineConfig> config = std::make_unique<GEPipelineConfig>();
		config->vertex_shader = IDR_VERT_SKYBOX;
		config->fragment_shader = IDR_FRAG_SKYBOX;

		config->attrStride = sizeof(GESkyboxVertex);
		config->attrOffsets.resize(1);
		config->attrOffsets[0] = offsetof(GESkyboxVertex, pos);
		config->attrFormats.resize(1);
		config->attrFormats[0] = VK_FORMAT_R32G32B32_SFLOAT;

		config->descriptorTypes.resize(2);
		config->descriptorTypes[0] = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		config->descriptorTypes[1] = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

		config->descriptorStages.resize(2);
		config->descriptorStages[0] = VK_SHADER_STAGE_VERTEX_BIT;
		config->descriptorStages[1] = VK_SHADER_STAGE_FRAGMENT_BIT;

		config->depthWrite = VK_FALSE;
		config->depthTestEnable = VK_FALSE;
		config->cullMode = VK_CULL_MODE_NONE;
		config->extent = extent;

		return config;
	}


//
// FUNCIÓN: GEScene::createScenePipelineConfig()
//
// PROPÓSITO: Obtiene la configuración del pipeline de renderizado para las figuras
//
	std::unique_ptr <GEPipelineConfig> GEScene::createScenePipelineConfig(VkExtent2D extent)
{
	std::unique_ptr<GEPipelineConfig> config = std::make_unique<GEPipelineConfig>();
	config->vertex_shader = IDR_VERT;
	config->fragment_shader = IDR_FRAG;

	config->attrStride = sizeof(GEVertex);
	config->attrOffsets.resize(3);
	config->attrOffsets[0] = offsetof(GEVertex, pos);
	config->attrOffsets[1] = offsetof(GEVertex, norm);
	config->attrOffsets[2] = offsetof(GEVertex, tex);
	config->attrFormats.resize(3);
	config->attrFormats[0] = VK_FORMAT_R32G32B32_SFLOAT;
	config->attrFormats[1] = VK_FORMAT_R32G32B32_SFLOAT;
	config->attrFormats[2] = VK_FORMAT_R32G32_SFLOAT;


	config->descriptorTypes.resize(4);
	config->descriptorTypes[0] = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	config->descriptorTypes[1] = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	config->descriptorTypes[2] = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	config->descriptorTypes[3] = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	config->descriptorStages.resize(4);
	config->descriptorStages[0] = VK_SHADER_STAGE_ALL_GRAPHICS;
	config->descriptorStages[1] = VK_SHADER_STAGE_FRAGMENT_BIT;
	config->descriptorStages[2] = VK_SHADER_STAGE_FRAGMENT_BIT;
	config->descriptorStages[3] = VK_SHADER_STAGE_FRAGMENT_BIT;


	config->depthWrite = VK_TRUE;
	config->depthTestEnable = VK_TRUE;
	config->cullMode = VK_CULL_MODE_BACK_BIT;
	config->extent = extent;

	config->topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	return config;
}


//
// FUNCI�N: GEScene::createParticlePipelineConfig()
//
// PROP�SITO: Obtiene la configuraci�n del pipeline de particulass/ computacion
//
std::unique_ptr <GEPipelineConfig> GEScene::createParticlePipelineConfig(VkExtent2D extent)
{
	std::unique_ptr<GEPipelineConfig> config = std::make_unique<GEPipelineConfig>();

	config->vertex_shader = IDR_VERT_PARTICLES;   
	config->fragment_shader = IDR_FRAG_PARTICLES; 

	// Definici�n del Salto (Stride)
	config->attrStride = sizeof(GEParticle); 

	config->attrOffsets.resize(5);
	config->attrOffsets[0] = offsetof(GEParticle, pos);
	config->attrOffsets[1] = offsetof(GEParticle, norm);
	config->attrOffsets[2] = offsetof(GEParticle, color);
	config->attrOffsets[3] = offsetof(GEParticle, size);
	config->attrOffsets[4] = offsetof(GEParticle, activeTTL);

	config->attrFormats.resize(5);
	config->attrFormats[0] = VK_FORMAT_R32G32B32_SFLOAT;
	config->attrFormats[1] = VK_FORMAT_R32G32B32_SFLOAT;
	config->attrFormats[2] = VK_FORMAT_R32G32B32A32_SFLOAT;
	config->attrFormats[3] = VK_FORMAT_R32_SFLOAT;
	config->attrFormats[4] = VK_FORMAT_R32_SINT;


	config->descriptorTypes = {
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, // Transform (MVP)
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, // Material
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, // Light
		VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER   // Textura
	};
	config->descriptorStages = {
		VK_SHADER_STAGE_VERTEX_BIT,
		VK_SHADER_STAGE_FRAGMENT_BIT,
		VK_SHADER_STAGE_FRAGMENT_BIT,
		VK_SHADER_STAGE_FRAGMENT_BIT
	};

	config->topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST; // Dibujar puntos, no tri�ngulos
	
	config->depthWrite = VK_FALSE;
	config->depthTestEnable = VK_TRUE;
	config->cullMode = VK_CULL_MODE_NONE; // Los puntos no tienen cara trasera
	config->extent = extent;

	return config;
}