#include "GEApplication.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <windows.h>
#include <iostream>
#include <vector>
#include <glm/common.hpp>
#include "resource.h"
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

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

	if (scene->getRenderingContext() != nullptr) {
		std::cout << "Intentando iniciar imgui" << std::endl;
		inicializarImGui();
	}
	else {
		std::cout << "[ERROR FATAL] La escena no creó el RenderingContext" << std::endl;
	}

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
	// 1. SINCRONIZACIÓN (Esto limpia el camino para el Command Buffer)
	dc->waitForNextImage(gc.get());
	uint32_t i = dc->getCurrentImage();
	VkCommandBuffer cb = cc->commandBuffers[i];

	// 2. RESET Y BEGIN (Si esto falla, nada se dibuja)
	vkResetCommandBuffer(cb, 0);
	VkCommandBufferBeginInfo beginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; // Añade este flag

	if (vkBeginCommandBuffer(cb, &beginInfo) != VK_SUCCESS) return;

	// 3. PREPARAR IMGUI (Cálculos de vértices de la UI)
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Aquí puedes poner tus ventanas de ImGui: ImGui::Begin("Test"); ImGui::End();

	ImGui::Render(); // IMPORTANTE: Render() debe ir antes de RenderDrawData

	// 4. ACTUALIZAR Y DIBUJAR ESCENA
	scene->update(gc.get(), i);
	scene->recordComputeCommands(cb, i); // Tus partículas

	auto rc = scene->getRenderingContext();
	rc->insertBeginCommands(cb, i); // Inicia RenderPass

	scene->drawGraphicsObjects(cb, i);

	// 5. DIBUJAR IMGUI (Dentro del RenderPass, al final para que esté encima)
	ImDrawData* drawData = ImGui::GetDrawData();
	if (drawData) {
		ImGui_ImplVulkan_RenderDrawData(drawData, cb);
	}

	// 6. FINALIZAR
	rc->insertEndCommands(cb);
	vkEndCommandBuffer(cb);

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

	if (imguiPool != VK_NULL_HANDLE) {
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		vkDestroyDescriptorPool(gc->device, imguiPool, nullptr);
	}

	if(scene) scene->destroy(gc.get());
	if(cc) cc->destroy(gc.get());
	if(dc) dc->destroy(gc.get());
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

	this->cc = std::make_unique<GECommandContext>(this->gc.get(), this->dc->getImageCount());

	scene->recreate(gc.get(), dc.get(), cc.get());

	double aspect;
	if (!windowPos.fullScreen) aspect = (double)this->windowPos.width / (double)this->windowPos.height;
	else aspect = (double)this->windowPos.screenWidth / (double)this->windowPos.screenHeight;
	this->scene->aspect_ratio(aspect);
}

void GEApplication::inicializarImGui() {
	// 1. Comprobaciones de seguridad iniciales
	if (!gc) {
		std::cerr << "[ImGui] ERROR: GraphicsContext (gc) es nulo." << std::endl;
		return;
	}
	if (!scene) {
		std::cerr << "[ImGui] ERROR: La escena no existe. ImGui necesita el RenderPass de la escena." << std::endl;
		return;
	}

	auto renderContext = scene->getRenderingContext();
	if (!renderContext) {
		std::cerr << "[ImGui] ERROR: El RenderingContext es nulo. No se puede obtener el RenderPass." << std::endl;
		return;
	}

	// 2. Crear Pool de Descriptores con verificación
	VkDescriptorPoolSize pool_sizes[] = {
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 }
	};
	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	pool_info.maxSets = 1000;
	pool_info.poolSizeCount = (uint32_t)std::size(pool_sizes);
	pool_info.pPoolSizes = pool_sizes;

	if (vkCreateDescriptorPool(gc->device, &pool_info, nullptr, &this->imguiPool) != VK_SUCCESS) {
		std::cerr << "[ImGui] ERROR: No se pudo crear el Descriptor Pool." << std::endl;
		return;
	}

	// 3. Setup Contexto ImGui
	IMGUI_CHECKVERSION();
	if (!ImGui::CreateContext()) {
		std::cerr << "[ImGui] ERROR: No se pudo crear el contexto de ImGui." << std::endl;
		return;
	}

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();

	// 4. Inicializar Binding de GLFW
	if (!ImGui_ImplGlfw_InitForVulkan(this->window.get(), true)) {
		std::cerr << "[ImGui] ERROR: Falló la inicialización de ImGui para GLFW." << std::endl;
		return;
	}

	// 5. Configurar información de inicialización para Vulkan
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = gc->instance;
	init_info.PhysicalDevice = gc->physicalDevice;
	init_info.Device = gc->device;
	init_info.QueueFamily = gc->graphicsQueueFamilyIndex;

	// Obtenemos la cola de gráficos directamente
	vkGetDeviceQueue(gc->device, gc->graphicsQueueFamilyIndex, 0, &init_info.Queue);
	if (init_info.Queue == VK_NULL_HANDLE) {
		std::cerr << "[ImGui] ERROR: No se pudo obtener la cola (Queue) de Vulkan." << std::endl;
		return;
	}

	init_info.DescriptorPool = this->imguiPool;
	init_info.MinImageCount = 2;
	init_info.ImageCount = renderContext->getImageCount();

	// Configuración del Pipeline (Tu versión de ImGui usa PipelineInfoMain)
	init_info.PipelineInfoMain.RenderPass = renderContext->getRenderPass();
	init_info.PipelineInfoMain.Subpass = 0;
	init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

	// 6. Inicializar Binding de Vulkan
	if (!ImGui_ImplVulkan_Init(&init_info)) {
		std::cerr << "[ImGui] ERROR: Falló la inicialización de ImGui para Vulkan." << std::endl;
		return;
	}

	// MENSAJE DE ÉXITO
	std::cout << "--------------------------------------------------" << std::endl;
	std::cout << "[SUCCESS] ImGui inicializado correctamente!" << std::endl;
	std::cout << "[INFO] RenderPass utilizado: " << renderContext->getRenderPass() << std::endl;
	std::cout << "[INFO] Imágenes en Swapchain: " << renderContext->getImageCount() << std::endl;
	std::cout << "--------------------------------------------------" << std::endl;
}