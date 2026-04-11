#include "GEApplication.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <windows.h>
#include <iostream>
#include <vector>
#include <glm/common.hpp>
#include "resource.h"
#include "imgui.h"
#include "backends/imgui_impl_vulkan.h"
#include "backends/imgui_impl_glfw.h"

//
// FUNCIÓN: GEApplication::run()
//
// PROPÓSITO: Ejecuta la aplicación
//
void GEApplication::run()
{
	// Usamos reset para que el unique_ptr tome el control del puntero devuelto
	this->window.reset(initWindow());
	this->windowPos = initWindowPos();
	this->gc = std::make_unique<GEGraphicsContext>(window.get());
	this->dc = std::make_unique <GEDrawingContext>(this->gc.get(), this->windowPos);
	this->cc = std::make_unique < GECommandContext>(this->gc.get(), this->dc->getImageCount());

	this->scene = std::make_unique <GEScene>(gc.get(), dc.get(), cc.get());

	mainLoop();

	cleanup();
}

//
// FUNCIÓN: GEApplication::initWindow()
//
// PROPÓSITO: Inicializa la ventana
//
GLFWwindow* GEApplication::initWindow()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* w = glfwCreateWindow(WIDTH, HEIGHT, "Game Engine", nullptr, nullptr);
	glfwSetWindowUserPointer(w, this);
	glfwSetFramebufferSizeCallback(w, framebufferResizeCallback);
	glfwSetKeyCallback(w, keyCallback);
	glfwSetCursorPosCallback(w, cursorPositionCallback);
	glfwSetMouseButtonCallback(w, mouseButtonCallback);


	return w;
}

//
// FUNCIÓN: GEApplication::initWindowPos()
//
// PROPÓSITO: Inicializa la posición de la ventana
//
GEWindowPosition GEApplication::initWindowPos()
{
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	GEWindowPosition wp = {};
	wp.fullScreen = false;
	glfwGetWindowSize(window.get(), &wp.width, &wp.height);
	glfwGetWindowPos(window.get(), &wp.Xpos, &wp.Ypos);
	wp.screenWidth = mode->width;
	wp.screenHeight = mode->height;
	return wp;
}


//
// FUNCIÓN: GEApplication::mainLoop()
//
// PROPÓSITO: Bucle principal que procesa los eventos de la aplicación
//
void GEApplication::mainLoop()
{
	while (!glfwWindowShouldClose(window.get()))
	{
		glfwPollEvents();
		draw();
	}
}

//
// FUNCIÓN: GEApplication::draw()
//
// PROPÓSITO: Lanza la generación del dibujo
//
void GEApplication::draw()
{
	if (ImGui::GetCurrentContext() == nullptr) {
		throw std::runtime_error("¡CRASH EVITADO! ImGui no se ha inicializado. Revisa donde pusiste ImGui::CreateContext().");
	}

	dc->waitForNextImage(gc.get());


	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();


	scene->update(gc.get(), dc->getCurrentImage());

	ImGui::Render();

	scene->fillCommandBuffers(cc.get());

	dc->submitGraphicsCommands(gc.get(), cc->commandBuffers);
	dc->submitPresentCommands(gc.get());
}

//
// FUNCIÓN: GEApplication::cleanup()
//
// PROPÓSITO: Libera los recursos y finaliza la aplicación
//
void GEApplication::cleanup()
{
	if(gc) vkDeviceWaitIdle(gc->device); // ESPERA A LA GPU ANTES DE DESTRUIR NADA

	if(scene) scene->destroy(gc.get());
	if(cc) cc->destroy(gc.get());
	if(dc) dc->destroy(gc.get());
	//delete scene;
	//delete cc;
	//delete dc;
	//delete gc;
	//glfwDestroyWindow(window.get());
	glfwTerminate();
}

//
// FUNCIÓN: GEApplication::swapFullScreen()
//
// PROPÓSITO: Dibuja la ventana a pantalla completa o a tamaño configurable
//
void GEApplication::swapFullScreen()
{
	if (!windowPos.fullScreen)
	{
		glfwGetWindowSize(window.get(), &windowPos.width, &windowPos.height);
		glfwGetWindowPos(window.get(), &windowPos.Xpos, &windowPos.Ypos);
		windowPos.fullScreen = true;
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		glfwSetWindowMonitor(window.get(), monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
	}
	else
	{
		windowPos.fullScreen = false;
		glfwSetWindowMonitor(window.get(), nullptr, windowPos.Xpos, windowPos.Ypos, windowPos.width, windowPos.height, NULL);
	}
}

//
// FUNCIÓN: GEApplication::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
//
// PROPÓSITO: Respuesta a un evento de teclado sobre la aplicación
//
void GEApplication::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	GEApplication* app = (GEApplication*)glfwGetWindowUserPointer(window);
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		if (key == GLFW_KEY_F12) app->swapFullScreen();
		else app->scene->key_action(key, true);
	}
	else app->scene->key_action(key, false);
}

//
// FUNCIÓN: GEApplication::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
//
// PROPÓSITO: Respuesta a un evento de ratón sobre la aplicación
//
void GEApplication::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	GEApplication* app = (GEApplication*)glfwGetWindowUserPointer(window);
	if (app->scene) {
		app->scene->mouse_button_action(window, button, action);
	}
}

//
// FUNCIÓN: GEApplication::cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
//
// PROPÓSITO: Respuesta a un evento de movimiento del cursor sobre la aplicación
//
void GEApplication::cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
	GEApplication* app = (GEApplication*)glfwGetWindowUserPointer(window);
	if (app->scene) {
		app->scene->mouse_action(xpos, ypos);
	}
}

//
// FUNCIÓN: GEApplication::framebufferResizeCallback(GLFWwindow* window, int width, int height)
//
// PROPÓSITO: Respuesta a un evento de redimensionamiento de la ventana de la aplicación
//
void GEApplication::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	GEApplication* app = (GEApplication*)glfwGetWindowUserPointer(window);
	if (height != 0)
	{
		app->resize();
	}
}

//
// FUNCIÓN: GEApplication::resize()
//
// PROPÓSITO: Reconstruye los objetos con el nuevo tamaño de ventana
//
void GEApplication::resize()
{
	// Esperar a que la GPU termine TODO antes de destruir recursos en uso
	vkDeviceWaitIdle(gc->device);
	#ifdef _DEBUG
		std::cout << "DEBUG_INFO: Haciendo resize a la ventana." << std::endl;
	#endif
	if (!windowPos.fullScreen)
	{
		glfwGetWindowSize(window.get(), &windowPos.width, &windowPos.height);
		glfwGetWindowPos(window.get(), &windowPos.Xpos, &windowPos.Ypos);
	}

	dc->recreate(gc.get(), windowPos);
	cc->destroy(gc.get());
	// delete cc;

	this->cc = std::make_unique<GECommandContext>(this->gc.get(), this->dc->getImageCount());

	scene->recreate(gc.get(), dc.get(), cc.get());

	double aspect;
	if (!windowPos.fullScreen) aspect = (double)this->windowPos.width / (double)this->windowPos.height;
	else aspect = (double)this->windowPos.screenWidth / (double)this->windowPos.screenHeight;
	this->scene->aspect_ratio(aspect);
}

