/**
 * @file GEApplication.h
 * @brief Archivo GEApplication.h
 */
#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "GEWindowPosition.h"
#include "GEGraphicsContext.h"
#include "GEDrawingContext.h"
#include "GECommandContext.h"
#include "GEScene.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>


const int WIDTH = 800;
const int HEIGHT = 600;

// Estructura para decirle a unique_ptr como borrar la ventana de GLFW
/**
 * @struct GLFWWindowDeleter
 * @brief Struct GLFWWindowDeleter
 */
struct GLFWWindowDeleter {
	/**
	 * @brief FunciÃ³n operator
	 */
	void operator()(GLFWwindow* window) const {
		if (window) glfwDestroyWindow(window);
	}
};

//
// CLASE: GEApplication
//
// DESCRIPCION: Clase que crea y lanza la aplicacion grafica.
//
/**
 * @class GEApplication
 * @brief Class GEApplication
 */
class GEApplication
{
public:
	
	/**
	 * @brief FunciÃ³n run
	 */
	void run();

private:
	std::unique_ptr < GLFWwindow, GLFWWindowDeleter> window;
	GEWindowPosition windowPos;
	std::unique_ptr<GEGraphicsContext> gc;
	std::unique_ptr<GEDrawingContext> dc;
	std::unique_ptr<GECommandContext> cc;
	std::unique_ptr<GEScene> scene;

	// Metodos principales
	GLFWwindow* initWindow();
	/**
	 * @brief FunciÃ³n initWindowPos
	 */
	GEWindowPosition initWindowPos();

	VkDescriptorPool imguiPool = VK_NULL_HANDLE;

	VkQueryPool queryPool = VK_NULL_HANDLE;

	/**
	 * @brief FunciÃ³n mainLoop
	 */
	void mainLoop();
	/**
	 * @brief FunciÃ³n draw
	 */
	void draw(double deltaTime, int physicsSteps, double alpha, double frameTime);
	/**
	 * @brief FunciÃ³n cleanup
	 */
	void cleanup();
	/**
	 * @brief FunciÃ³n swapFullScreen
	 */
	void swapFullScreen();
	/**
	 * @brief FunciÃ³n resize
	 */
	void resize();

	// Respuesta a eventos
	/**
	 * @brief FunciÃ³n keyCallback
	 */
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	/**
	 * @brief FunciÃ³n mouseButtonCallback
	 */
	static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	/**
	 * @brief FunciÃ³n cursorPositionCallback
	 */
	static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
	/**
	 * @brief FunciÃ³n framebufferResizeCallback
	 */
	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

	/**
	 * @brief FunciÃ³n inicializarImGui
	 */
	void inicializarImGui();

	/**
	 * @brief FunciÃ³n ControlsGUI
	 */
	void ControlsGUI(GECamera* cam);

};
