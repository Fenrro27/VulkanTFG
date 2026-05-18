/**

 * @file GEScene.cpp

 * @brief Archivo GEScene.cpp

 */

#define TINYOBJLOADER_IMPLEMENTATION

#include "tiny_obj_loader.h"

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

#ifdef _WIN32
#include <windows.h>
#endif
#include "resource.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

#include "GEHumo.h"
#include "GEFuego.h"
#include "GEAgua.h"
#include "GETexture.h"
#include "GEModel.h"
#include "commonDebug.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

//

// FUNCIï¿½N: GEScene::GEScene(GEGraphicsContext* gc, GEDrawingContext* dc)

//

// PROPï¿½SITO: Crea la escena

//

GEScene::GEScene(GEGraphicsContext *gc, GEDrawingContext *dc, GECommandContext *cc)

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

	this->camera = std::make_unique<GECamera>();

	this->camera->setPosition(glm::vec3(0.0f, 10.0f, 100.0f));

	this->camera->setMoveStep(0.0f);

	rc->setActivePipeline(SKYBOX_PIPELINE);

	this->skybox = std::make_unique<GESkybox>(gc, rc.get());

	rc->setActivePipeline(SCENE_PIPELINE);

	auto texWood = std::make_shared<GETexture>(gc, "textures/wood.jpg");

	textures.push_back(texWood);

	auto texMoon = std::make_shared<GETexture>(gc, "textures/moon.jpg");

	textures.push_back(texMoon);

	auto texSmoke = std::make_shared<GETexture>(gc, "textures/smoke.png");

	textures.push_back(texSmoke);

	auto texFire = std::make_shared<GETexture>(gc, "textures/fire.png");

	textures.push_back(texFire);

	auto texWater = std::make_shared<GETexture>(gc, "textures/pngwing.com (1).png");

	textures.push_back(texWater);

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

	std::unique_ptr<GEFigure> ground = std::make_unique<GEGround>(150.0f, 150.0f);

	ground->setTexture(texWood.get()); // Obtiene el puntero crudo del unique_ptr

	ground->initialize(gc, rc.get());

	ground->setMaterial(groundMat);

	ground->setLight(light);

	figures.push_back(std::move(ground));

	// Carga de modelos .obj usando tinyobjloader

	auto fuente = std::make_unique<GEModel>(gc, "models/newFountain/fountain.obj", 0.5f);

	fuente->initialize(gc, rc.get());
	fuente->translate(glm::vec3(25.0f, 0.0f, 0.0f));
	fuente->setLight(light);
	objects.push_back(std::move(fuente));

	auto hoguera = std::make_unique<GEModel>(gc, "models/campfire/campfire.obj", 0.01f);

	hoguera->initialize(gc, rc.get());

	// hoguera->rotate(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f));

	hoguera->translate(glm::vec3(0.0f, -0.1f, 0.0f));

	hoguera->setLight(light);

	objects.push_back(std::move(hoguera));

	auto tren = std::make_unique<GEModel>(gc, "models/train/sketchfabpreview.obj", 0.1f);

	tren->initialize(gc, rc.get());

	// tren->rotate(-90.0f, glm::vec3(1.0f, 0.0f, 0.0f));

	tren->translate(glm::vec3(-25.0f, 0.0f, -6.5f));

	tren->setLight(light);

	objects.push_back(std::move(tren));

	rc->setActivePipeline(PARTICLE_PIPELINE);

	// particleCompute = std::make_unique<GEComputeShader>(gc, IDR_COMPUTE_PARTICLES, dc->getImageCount());

	computeShaders.push_back(std::make_unique<GEComputeShader>(gc, IDR_COMPUTE_HUMO, dc->getImageCount()));

	computeShaders.push_back(std::make_unique<GEComputeShader>(gc, IDR_COMPUTE_FUEGO, dc->getImageCount()));

	computeShaders.push_back(std::make_unique<GEComputeShader>(gc, IDR_COMPUTE_AGUA, dc->getImageCount()));

	particleSystem.push_back(std::make_unique<GEHumo>(100));

	particleSystem.push_back(std::make_unique<GEFuego>(100));

	particleSystem.push_back(std::make_unique<GEAgua>(1000));

	GEMaterial particle1Mat = {};

	particle1Mat.Ka = glm::vec3(0.1f, 0.1f, 0.1f);

	particle1Mat.Kd = glm::vec3(0.4f, 0.42f, 0.45f);

	particle1Mat.Ks = glm::vec3(0.05f, 0.05f, 0.05f);

	particle1Mat.Shininess = 2.0f;

	particleSystem[0]->initialize(gc, rc.get(), texSmoke.get());

	particleSystem[0]->translate(glm::vec3(-25.0f, 12.0f, 0.0f));

	particleSystem[0]->setMaterial(particle1Mat);

	particleSystem[0]->setLight(light);

	computeShaders[0]->addParticleSystem(gc, dc->getImageCount(), particleSystem[0].get());

	camera->addObservationPoint(particleSystem[0]->getLocation(), "Humo");

	particleSystem[1]->initialize(gc, rc.get(), texFire.get());

	particleSystem[1]->translate(glm::vec3(-1.0f, 0.5f, 1.0f));

	particleSystem[1]->setMaterial(particle1Mat);

	particleSystem[1]->setLight(light);

	computeShaders[1]->addParticleSystem(gc, dc->getImageCount(), particleSystem[1].get());

	camera->addObservationPoint(particleSystem[1]->getLocation(), "Fuego");

	particleSystem[2]->initialize(gc, rc.get(), texWater.get());

	particleSystem[2]->translate(glm::vec3(25.0f, 12.8f, 0.0f));

	particleSystem[2]->setMaterial(particle1Mat);

	particleSystem[2]->setLight(light);

	computeShaders[2]->addParticleSystem(gc, dc->getImageCount(), particleSystem[2].get());

	camera->addObservationPoint(particleSystem[2]->getLocation(), "Agua");
}

//

// FUNCIï¿½N: GEScene::destroy(GEGraphicsContext* gc)

//

// PROPï¿½SITO: Reconstruye los componentes grï¿½ficos de la escena

//

/**

 * @brief FunciÃ³n GEScene::destroy

 */

void GEScene::destroy(GEGraphicsContext *gc)

{

	for (auto &cs : computeShaders)
	{

		cs->destroy(gc);
	}

	rc->destroy(gc);

	skybox->destroy(gc);

	for (auto &figure : figures)

	{

		figure->destroy(gc);
	}

	for (auto &ob : objects)

	{

		ob->destroy(gc);
	}

	for (auto &ps : particleSystem)
	{

		ps->destroy(gc);
	}

	particleSystem.clear();

	for (auto &tex : textures)
	{

		if (tex != nullptr)
		{

			tex->destroy(gc);
		}
	}

	textures.clear();
}

//

// FUNCIï¿½N: GEScene::recreate(GEGraphicsContext* gc, GEDrawingContext* dc)

//

// PROPï¿½SITO: Reconstruye los componentes grï¿½ficos de la escena

//

/**

 * @brief FunciÃ³n GEScene::recreate

 */

void GEScene::recreate(GEGraphicsContext *gc, GEDrawingContext *dc, GECommandContext *cc)

{

	rc->destroy(gc);

	auto skybox_config = createSkyboxPipelineConfig(dc->getExtent());

	this->rc = std::make_unique<GERenderingContext>(gc, dc, skybox_config.get());

	auto scene_config = createScenePipelineConfig(dc->getExtent());

	this->rc->addGraphicsPipeline(gc, scene_config.get());

	auto particle_Config = createParticlePipelineConfig(dc->getExtent());

	this->rc->addGraphicsPipeline(gc, particle_Config.get());

	//	fillCommandBuffers(cc);
}

//

// FUNCIï¿½N: GEScene::update(GEGraphicsContext* gc, uint32_t index)

//

// PROPï¿½SITO: Actualiza la informaciï¿½n para generar la imagen

//

/**

 * @brief FunciÃ³n GEScene::update

 */

void GEScene::update(GEGraphicsContext *gc, uint32_t index, float deltaTime)

{

	camera->update(deltaTime);

	glm::mat4 view = camera->getViewMatrix();

	skybox->update(gc, index, view, projection);

	for (auto &figure : figures)

	{

		figure->update(gc, index, view, projection);
	}

	for (auto &ob : objects)

	{

		ob->update(gc, index, view, projection);
	}

	for (auto &ps : particleSystem)

	{

		ps->update(gc, index, view, projection);
	}
}

//

// FUNCIï¿½N: GEScene::key_action(int key, bool pressed)

//

// PROPï¿½SITO: Respuesta a acciones de teclado

//

/**

 * @brief FunciÃ³n GEScene::key_action

 */

void GEScene::key_action(int key, bool pressed)

{

	// Cambiar de modo siempre funcionarÃ¡ con M

	if (key == GLFW_KEY_M && pressed)
	{

		camera->stopAllMovement();

		camera->setNextMode();

		firstMouse = true;

		GE_DEBUG_INFO("Modo camara alternado");

		return; // Salimos de la funciÃ³n si cambiamos de modo
	}

	// Comportamiento dependiendo del modo en el que estemos

	auto mode = camera->getCurrentMode();

	if (mode == GECamera::CameraMode::FREE)
	{

		// Movimiento Normal (NO TOCADO)

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
			if (pressed)
				camera->setMoveStep(0.0f);
			break;

		case GLFW_KEY_KP_ADD:

		case GLFW_KEY_1:
			if (pressed)
				camera->setMoveStep(camera->getMoveStep() + 0.10f);
			break;

		case GLFW_KEY_KP_SUBTRACT:

		case GLFW_KEY_2:
			if (pressed)
				camera->setMoveStep(camera->getMoveStep() - 0.10f);
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

	else if (mode == GECamera::CameraMode::FPS)
	{

		if (pressed)
			camera->setMoveStep(3.0f);

		// Movimiento FPS: WASD para moverse, Espacio y Shift para subir/bajar

		switch (key)
		{

		case GLFW_KEY_W:
			camera->setMoveFront(pressed);
			break;

		case GLFW_KEY_S:
			camera->setMoveBack(pressed);
			break;

		case GLFW_KEY_A:
			camera->setMoveLeft(pressed);
			break;

		case GLFW_KEY_D:
			camera->setMoveRight(pressed);
			break;

		case GLFW_KEY_SPACE:
			camera->setMoveUp(pressed);
			break;

		case GLFW_KEY_LEFT_SHIFT:
			camera->setMoveDown(pressed);
			break;
		}
	}

	else if (mode == GECamera::CameraMode::OBSERVING)
	{

		// Modo ObservaciÃ³n: Cambiar objetivo y acercar/alejar la cÃ¡mara

		if (pressed)
		{

			switch (key)
			{

			case GLFW_KEY_RIGHT:

				camera->nextObservationPoint();

				break;

			case GLFW_KEY_LEFT:

				camera->prevObservationPoint();

				break;

			case GLFW_KEY_UP:

				// Alejar la cÃ¡mara

				camera->setObservationDistance(camera->getObservationDistance() + 1.0f);

				break;

			case GLFW_KEY_DOWN:

				// Acercar la cÃ¡mara

				camera->setObservationDistance(camera->getObservationDistance() - 1.0f);

				// Evitamos que la cÃ¡mara atraviese el centro del objeto por accidente

				if (camera->getObservationDistance() < 1.0f)
					camera->setObservationDistance(1.0f);

				break;
			}
		}
	}
}

/**

 * @brief FunciÃ³n GEScene::mouse_action

 */

void GEScene::mouse_action(double xpos, double ypos)

{

	// Si no estamos haciendo clic, no hacemos nada de cÃ¡lculo

	if (!isDragging)
		return;

	if (firstMouse)
	{

		lastX = (float)xpos;

		lastY = (float)ypos;

		firstMouse = false;
	}

	float xoffset = (float)xpos - lastX;

	float yoffset = lastY - (float)ypos; // Invertido

	lastX = (float)xpos;

	lastY = (float)ypos;

	camera->processMouse(xoffset, yoffset);
}

//

// FUNCIï¿½N: GEScene::aspect_ratio(double)

//

// PROPï¿½SITO: Modifica la relaciï¿½n anchura/altura del modelo

//

/**

 * @brief FunciÃ³n GEScene::aspect_ratio

 */

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

//

// FUNCIÃN: GEScene::createSkyboxPipelineConfig()

//

// PROPÃSITO: Obtiene la configuraciÃ³n del pipeline de renderizado para el skybox

//

std::unique_ptr<GEPipelineConfig> GEScene::createSkyboxPipelineConfig(VkExtent2D extent)

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

// FUNCIÃN: GEScene::createScenePipelineConfig()

//

// PROPÃSITO: Obtiene la configuraciÃ³n del pipeline de renderizado para las figuras

//

std::unique_ptr<GEPipelineConfig> GEScene::createScenePipelineConfig(VkExtent2D extent)

{

	std::unique_ptr<GEPipelineConfig> config = std::make_unique<GEPipelineConfig>();

	config->vertex_shader = IDR_VERT_SCENE;

	config->fragment_shader = IDR_FRAG_SCENE;

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

// FUNCIï¿½N: GEScene::createParticlePipelineConfig()

//

// PROPï¿½SITO: Obtiene la configuraciï¿½n del pipeline de particulass/ computacion

//

std::unique_ptr<GEPipelineConfig> GEScene::createParticlePipelineConfig(VkExtent2D extent)

{

	std::unique_ptr<GEPipelineConfig> config = std::make_unique<GEPipelineConfig>();

	config->vertex_shader = IDR_VERT_PARTICLES;

	config->fragment_shader = IDR_FRAG_PARTICLES;

	// Definiciï¿½n del Salto (Stride)

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

		VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER // Textura

	};

	config->descriptorStages = {

		VK_SHADER_STAGE_VERTEX_BIT,

		VK_SHADER_STAGE_FRAGMENT_BIT,

		VK_SHADER_STAGE_FRAGMENT_BIT,

		VK_SHADER_STAGE_FRAGMENT_BIT

	};

	config->topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST; // Dibujar puntos, no triï¿½ngulos

	config->depthWrite = VK_FALSE;

	config->depthTestEnable = VK_TRUE;

	config->cullMode = VK_CULL_MODE_NONE; // Los puntos no tienen cara trasera

	config->extent = extent;

	return config;
}

/**

 * @brief FunciÃ³n GEScene::mouse_button_action

 */

void GEScene::mouse_button_action(GLFWwindow *window, int button, int action)

{

	if (button == GLFW_MOUSE_BUTTON_LEFT)

	{

		if (action == GLFW_PRESS)
		{

			isDragging = true;

			firstMouse = true;

			// Oculta el cursor y lo atrapa en la ventana (Movimiento infinito)

			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}

		else if (action == GLFW_RELEASE)
		{

			isDragging = false;

			// Vuelve a mostrar el cursor y lo libera

			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}
}

void GEScene::recordComputeCommands(VkCommandBuffer cb, uint32_t i,

									VkQueryPool queryPool, int physicsSteps)

{

	vkCmdResetQueryPool(cb, queryPool, 0, 2);

	vkCmdWriteTimestamp(cb, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, queryPool, 0);

	for (int step = 0; step < physicsSteps; step++)

	{

		// Despachar todos los sistemas en este paso

		for (size_t s = 0; s < particleSystem.size(); s++)

		{

			if (!particleSystem[s] || s >= computeShaders.size() || !computeShaders[s])

				continue;

			uint32_t groupCount = (particleSystem[s]->getParticlesCount() + 255) / 256;

			computeShaders[s]->recordCommands(cb, 0, i, groupCount);
		}

		// Si hay mÃ¡s pasos, barrera COMPUTEâCOMPUTE para encadenar correctamente

		if (step < physicsSteps - 1)

		{

			std::vector<VkBufferMemoryBarrier> stepBarriers;

			stepBarriers.reserve(particleSystem.size() * 2);

			for (auto &ps : particleSystem)

			{

				VkBufferMemoryBarrier b{};

				b.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;

				b.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;

				b.dstAccessMask = VK_ACCESS_SHADER_READ_BIT; // â COMPUTE lee en el sig. paso

				b.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

				b.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

				b.offset = 0;

				b.size = VK_WHOLE_SIZE;

				b.buffer = (i % 2 == 0) ? ps->getParticlesBufferB()->buffer

										: ps->getParticlesBufferA()->buffer;

				stepBarriers.push_back(b);
			}

			vkCmdPipelineBarrier(cb,

								 VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,

								 VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,

								 0, 0, nullptr,

								 (uint32_t)stepBarriers.size(), stepBarriers.data(),

								 0, nullptr);
		}
	}

	// Barrera final COMPUTEâVERTEX para que el vertex shader lea el resultado

	std::vector<VkBufferMemoryBarrier> finalBarriers;

	finalBarriers.reserve(particleSystem.size());

	for (auto &ps : particleSystem)

	{

		VkBufferMemoryBarrier b{};

		b.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;

		b.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;

		b.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;

		b.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		b.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		b.offset = 0;

		b.size = VK_WHOLE_SIZE;

		b.buffer = (i % 2 == 0) ? ps->getParticlesBufferB()->buffer

								: ps->getParticlesBufferA()->buffer;

		finalBarriers.push_back(b);
	}

	vkCmdPipelineBarrier(cb,

						 VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,

						 VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,

						 0, 0, nullptr,

						 (uint32_t)finalBarriers.size(), finalBarriers.data(),

						 0, nullptr);

	vkCmdWriteTimestamp(cb, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, queryPool, 1);
}

/**

 * @brief FunciÃ³n GEScene::drawGraphicsObjects

 */

void GEScene::drawGraphicsObjects(VkCommandBuffer cb, uint32_t i)

{

	// Skybox

	rc->setActivePipeline(SKYBOX_PIPELINE);

	vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, rc->getActivePipeline());

	skybox->addCommands(cb, rc->getActivePipelineLayout(), i);

	// Objetos de la escena

	rc->setActivePipeline(SCENE_PIPELINE);

	vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, rc->getActivePipeline());

	for (auto &figure : figures)
	{

		figure->addCommands(cb, rc->getActivePipelineLayout(), i);
	}

	for (auto &ob : objects)
	{

		ob->addCommands(cb, rc->getActivePipelineLayout(), i);
	}

	// PartÃ­culas

	rc->setActivePipeline(PARTICLE_PIPELINE);

	vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, rc->getActivePipeline());

	for (auto &ps : particleSystem)

	{

		uint32_t particleCount = ps->getParticlesCount();

		VkDescriptorSet ds = ps->getDescriptorSet(i);

		vkCmdBindDescriptorSets(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, rc->getActivePipelineLayout(), 0, 1, &ds, 0, nullptr);

		VkDeviceSize offset = 0;

		VkBuffer bufferADibujar = (i % 2 == 0) ? ps->getParticlesBufferB()->buffer : ps->getParticlesBufferA()->buffer;

		vkCmdBindVertexBuffers(cb, 0, 1, &bufferADibujar, &offset);

		vkCmdDraw(cb, particleCount, 1, 0, 0);
	}
}

/**

 * @brief FunciÃ³n GEScene::getTotalParticleCount

 */

uint32_t GEScene::getTotalParticleCount() const

{

	uint32_t total = 0;

	for (const auto &ps : particleSystem)
	{

		total += ps->getParticlesCount();
	}

	return total;
}

/**

 * @brief FunciÃ³n GEScene::updatePhysics

 */

void GEScene::updatePhysics(GEGraphicsContext *gc, uint32_t index, float fixedDeltaTime)
{

	// Actualizamos los buffers de los sistemas de partÃ­culas con el tiempo fijo

	for (auto &ps : particleSystem)
	{

		ps->updatePhysics(gc, index, fixedDeltaTime);
	}
}