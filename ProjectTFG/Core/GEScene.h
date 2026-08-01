/**
 * @file GEScene.h
 * @brief Archivo GEScene.h
 */
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

#include <string>

#include <mutex>

#include <exception>

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
/**
 * @class GEScene
 * @brief Class GEScene
 */
class GEScene
{
private:
	std::unique_ptr<GERenderingContext> rc;
	std::unique_ptr<GESkybox> skybox;
	std::unique_ptr<GECamera> camera;

	std::vector<std::unique_ptr<GEComputeShader>> computeShaders;

	//GEObject* plane; 
	std::vector<std::unique_ptr<GEFigure>> figures;
	std::vector<std::unique_ptr<GEObject>> objects;
	std::vector< std::unique_ptr<GEParticlesSystem>> particleSystem;
	
	std::vector< std::shared_ptr<GETexture>> textures;

	glm::mat4 projection;
	uint32_t particleCount = 10000;


	bool isDragging = false;
	bool firstMouse = true;
	float lastX = 400.0f; // Mitad de pantalla 800x600

	float lastY = 300.0f;

	// Estado de carga en segundo plano

	bool isLoadingState = false;

	float loadProgress = 0.0f;

	std::string loadMessage;

	mutable std::mutex loadMutex;

	std::exception_ptr loadError;

public:
	GEScene(GEGraphicsContext* gc, GEDrawingContext* dc, GECommandContext* cc);

	/**
	 * @brief Carga los recursos pesados de la escena en segundo plano
	 */
	void loadAssets(GEGraphicsContext* gc, GEDrawingContext* dc, GECommandContext* cc);

	/**
	 * @brief Marca si la escena esta cargando recursos en segundo plano
	 */
	void setLoading(bool loading);

	/**
	 * @brief Devuelve si la escena esta cargando recursos
	 */
	bool isLoading() const;

	/**
	 * @brief Actualiza el progreso de carga y el mensaje asociado
	 */
	void setLoadProgress(float progress, const std::string& message);

	/**
	 * @brief Devuelve el progreso de carga (0.0 a 1.0)
	 */
	float getLoadProgress() const;

	/**
	 * @brief Devuelve el mensaje de carga actual
	 */
	std::string getLoadMessage() const;

	/**
	 * @brief Almacena el error producido durante la carga en segundo plano
	 */
	void setLoadError(std::exception_ptr error);

	/**
	 * @brief Devuelve si hubo un error durante la carga
	 */
	bool hasLoadError() const;

	/**
	 * @brief Devuelve el error producido durante la carga
	 */
	std::exception_ptr getLoadError() const;

	/**
	 * @brief Función destroy
	 */
	void destroy(GEGraphicsContext* gc);
	/**
	 * @brief Función recreate
	 */
	void recreate(GEGraphicsContext* gc, GEDrawingContext* dc, GECommandContext* cc);
	/**
	 * @brief Función update
	 */
	void update(GEGraphicsContext* gc, uint32_t index, float deltaTime);
	/**
	 * @brief Función key_action
	 */
	void key_action(int key, bool pressed);
	/**
	 * @brief Función aspect_ratio
	 */
	void aspect_ratio(double aspect);

	/**
	 * @brief Función mouse_action
	 */
	void mouse_action(double xpos, double ypos);
	/**
	 * @brief Función toggle_camera_mode
	 */
	void toggle_camera_mode();
	/**
	 * @brief Función mouse_button_action
	 */
	void mouse_button_action(GLFWwindow* window, int button, int action);

	GERenderingContext* getRenderingContext() { return rc.get(); }
	/**
	 * @brief Función updatePhysics
	 */
	void updatePhysics(GEGraphicsContext* gc, uint32_t index, float fixedDeltaTime);

	/**
	 * @brief Función recordComputeCommands
	 */
	void recordComputeCommands(VkCommandBuffer cb, uint32_t i, VkQueryPool queryPool, int physicsSteps);
	/**
	 * @brief Función drawGraphicsObjects
	 */
	void drawGraphicsObjects(VkCommandBuffer cb, uint32_t i);
	GECamera* getCamera() { return camera.get(); }

	/**
	 * @brief Función getTotalParticleCount
	 */
	uint32_t getTotalParticleCount() const;

private:
	std::unique_ptr < GEPipelineConfig> createSkyboxPipelineConfig(VkExtent2D extent);
	std::unique_ptr < GEPipelineConfig> createScenePipelineConfig(VkExtent2D extent);
	std::unique_ptr < GEPipelineConfig> createParticlePipelineConfig(VkExtent2D extent);

};

