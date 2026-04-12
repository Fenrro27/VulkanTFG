#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "GEWindowPosition.h"
#include "GEGraphicsContext.h"
#include "GEDrawingContext.h"
#include "GECommandContext.h"
#include "GEScene.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>


const int WIDTH = 800;
const int HEIGHT = 600;

// Estructura para decirle a unique_ptr cómo borrar la ventana de GLFW
struct GLFWWindowDeleter {
	void operator()(GLFWwindow* window) const {
		if (window) glfwDestroyWindow(window);
	}
};

//
// CLASE: GEApplication
//
// DESCRIPCIÓN: Clase que crea y lanza la aplicación gráfica.
//
class GEApplication
{
public:
	void run();

private:
	std::unique_ptr < GLFWwindow, GLFWWindowDeleter> window;
	GEWindowPosition windowPos;
	std::unique_ptr<GEGraphicsContext> gc;
	std::unique_ptr<GEDrawingContext> dc;
	std::unique_ptr<GECommandContext> cc;
	std::unique_ptr<GEScene> scene;

	// Métodos principales
	GLFWwindow* initWindow();
	GEWindowPosition initWindowPos();

	VkDescriptorPool imguiPool = VK_NULL_HANDLE;

	void mainLoop();
	void draw();
	void cleanup();
	void swapFullScreen();
	void resize();

	// Respuesta a eventos
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

	void inicializarImGui();

};

