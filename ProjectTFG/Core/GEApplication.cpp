/**
 * @file GEApplication.cpp
 * @brief Archivo GEApplication.cpp
 */
#include "GEApplication.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "GEVulkanRuntime.h"

#ifdef _WIN32
#include <windows.h>
#endif
#include <iostream>
#include <vector>
#include <cstring>
#include <glm/common.hpp>
#include "resource.h"
#ifdef _WIN32
// Other windows specific headers if needed
#endif
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <imgui_internal.h>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <exception>


//
// FUNCION: GEApplication::run()
//
// PROPOSITO: Ejecuta la aplicacion
//
/**
 * @brief Función GEApplication::run
 */
void GEApplication::run()
{
	// Preparamos el entorno Vulkan (directorio de recursos y driver empaquetado)
	// antes de crear la ventana y el contexto grafico.
	GEVulkanRuntime::setup();

	// Usamos reset para que el unique_ptr tome el control del puntero devuelto
	this->window.reset(initWindow());
	this->windowPos = initWindowPos();
	this->gc = std::make_unique<GEGraphicsContext>(window.get());
	this->dc = std::make_unique <GEDrawingContext>(this->gc.get(), this->windowPos);
	this->cc = std::make_unique < GECommandContext>(this->gc.get(), this->dc->getImageCount());

	VkQueryPoolCreateInfo queryPoolInfo{};
	queryPoolInfo.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
	queryPoolInfo.queryType = VK_QUERY_TYPE_TIMESTAMP;
	queryPoolInfo.queryCount = 2; // Necesitamos 2, uno para el inicio y otro para el fin

	if (vkCreateQueryPool(gc->device, &queryPoolInfo, nullptr, &this->queryPool) != VK_SUCCESS) {
		std::cout << "[ERROR FATAL] Fallo al crear el Query Pool." << std::endl;
	}

	this->scene = std::make_unique <GEScene>(gc.get(), dc.get(), cc.get());

	if (scene->getRenderingContext() != nullptr) {
		std::cout << "Intentando iniciar imgui" << std::endl;
		inicializarImGui();
	}
	else {
		std::cout << "[ERROR FATAL] La escena no creo el RenderingContext" << std::endl;
	}

	// Cargamos los recursos pesados de la escena en un hilo en segundo plano,
	// mientras el hilo principal muestra una pantalla de carga animada.
	scene->setLoading(true);

	loaderThread = std::thread([this]() {
		try {
			scene->loadAssets(gc.get(), dc.get(), cc.get());
		}
		catch (...) {
			scene->setLoading(false);
			scene->setLoadError(std::current_exception());
		}
	});

	loadingLoop();

	if (loaderThread.joinable()) loaderThread.join();

	// Reaplicamos un resize que haya podido ocurrir durante la carga
	if (pendingResize) {
		pendingResize = false;
		resize();
	}

	if (scene->hasLoadError()) {
		std::rethrow_exception(scene->getLoadError());
	}

	if (glfwWindowShouldClose(window.get())) {
		cleanup();
		return;
	}

	mainLoop();


	cleanup();
}

//
// FUNCION: GEApplication::initWindow()
//
// PROPOSITO: Inicializa la ventana
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
// FUNCION: GEApplication::initWindowPos()
//
// PROPOSITO: Inicializa la posicion de la ventana
//
/**
 * @brief FunciÃ³n GEApplication::initWindowPos
 */
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
// FUNCION: GEApplication::mainLoop()
//
// PROPOSITO: Bucle principal que procesa los eventos de la aplicacion
//
/**
 * @brief FunciÃ³n GEApplication::mainLoop
 */
void GEApplication::mainLoop()
{
	using clock = std::chrono::steady_clock;

	auto lastTime = clock::now();
	double accumulator = 0.0;

	const double fixedDeltaTime = 1.0 / 60.0;

	while (!glfwWindowShouldClose(window.get()))
	{
		glfwPollEvents();

		auto currentTime = clock::now();
		double frameTime = std::chrono::duration<double>(currentTime - lastTime).count();
		lastTime = currentTime;

		if (frameTime > 0.25) frameTime = 0.25;

		accumulator += frameTime;


		int physicsSteps = 0;
		while (accumulator >= fixedDeltaTime) {
			physicsSteps++;
			accumulator -= fixedDeltaTime;
		}

		double alpha = accumulator / fixedDeltaTime;

		draw(fixedDeltaTime, physicsSteps, alpha, frameTime);
	}
}

//
// FUNCION: GEApplication::loadingLoop()
//
// PROPOSITO: Bucle que renderiza la pantalla de carga mientras la escena se carga en segundo plano
//
/**
 * @brief Función GEApplication::loadingLoop
 */
void GEApplication::loadingLoop()
{
	using clock = std::chrono::steady_clock;

	auto lastTime = clock::now();
	auto startTime = lastTime;
	const double minLoadingTime = 1.5;

	// Mostramos la pantalla de carga mientras el hilo trabaja y,
	// como minimo, durante minLoadingTime para que la animacion sea visible.
	while ((scene->isLoading() || std::chrono::duration<double>(clock::now() - startTime).count() < minLoadingTime)
		&& !glfwWindowShouldClose(window.get()))
	{
		glfwPollEvents();

		auto currentTime = clock::now();
		double frameTime = std::chrono::duration<double>(currentTime - lastTime).count();
		lastTime = currentTime;

		if (frameTime > 0.25) frameTime = 0.25;

		drawLoading(frameTime);
	}

	glfwPollEvents();
}

//
// FUNCION: GEApplication::drawLoading(double frameTime)
//
// PROPOSITO: Renderiza un frame mostrando unicamente la pantalla de carga
//
/**
 * @brief Función GEApplication::drawLoading
 */
void GEApplication::drawLoading(double frameTime)
{
	// 1. SINCRONIZACION
	if (!dc->waitForNextImage(gc.get())) {
		return;
	}
	uint32_t i = dc->getCurrentImage();
	VkCommandBuffer cb = cc->commandBuffers[i];

	// 2. RESET Y BEGIN
	vkResetCommandBuffer(cb, 0);
	VkCommandBufferBeginInfo beginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	if (vkBeginCommandBuffer(cb, &beginInfo) != VK_SUCCESS) return;

	// 3. PREPARAR IMGUI
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// 4. PANTALLA DE CARGA
	renderLoadingScreen();

	// 5. DIBUJAR IMGUI
	ImGui::Render();

	auto rc = scene->getRenderingContext();
	rc->insertBeginCommands(cb, i);

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
// FUNCION: GEApplication::renderLoadingScreen()
//
// PROPOSITO: Dibuja la interfaz de la pantalla de carga (titulo, spinner y barra de progreso)
//
/**
 * @brief Función GEApplication::renderLoadingScreen
 */
void GEApplication::renderLoadingScreen()
{
	ImGuiIO& io = ImGui::GetIO();
	ImVec2 screen = io.DisplaySize;

	float cx = screen.x * 0.5f;
	float cy = screen.y * 0.45f;

	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
	ImGui::SetNextWindowSize(screen);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.05f, 0.05f, 0.09f, 1.0f));

	ImGuiWindowFlags flags =
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoSavedSettings;

	ImGui::Begin("LoadingScreen", nullptr, flags);
	{
		// Titulo (grande, rasterizado a tamano real para que se vea suave)
		ImDrawList* dl = ImGui::GetWindowDrawList();
		const char* title = "VULKAN TFG";
		ImFont* titleFont = (bigTitleFont != nullptr) ? bigTitleFont : ImGui::GetFont();
		float titleFontSize = titleFont->FontSize;
		ImVec2 titleSize = titleFont->CalcTextSizeA(titleFontSize, FLT_MAX, 0.0f, title);
		ImU32 titleCol = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 0.75f, 0.3f, 1.0f));
		dl->AddText(titleFont, titleFontSize, ImVec2(cx - titleSize.x * 0.5f, cy - 130.0f), titleCol, title);

		// Spinner animado (puntos orbitando)
		float t = (float)ImGui::GetTime();
		const int dots = 8;
		const float radius = 18.0f;
		const float PI2 = 6.2831853f;
		for (int i = 0; i < dots; i++) {
			float a = t * 2.4f + (float)i * (PI2 / dots);
			float x = cx + std::cos(a) * radius;
			float y = cy + std::sin(a) * radius;
			float fade = 0.25f + 0.75f * (0.5f + 0.5f * std::sin((float)i - t * 5.0f));
			ImU32 col = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 0.72f, 0.3f, fade));
			dl->AddCircleFilled(ImVec2(x, y), 4.0f, col, 16);
		}

		// Mensaje de carga
		std::string msg = scene->getLoadMessage();
		ImVec2 msgSize = ImGui::CalcTextSize(msg.c_str());
		ImGui::SetCursorPos(ImVec2(cx - msgSize.x * 0.5f, cy + 40.0f));
		ImGui::TextColored(ImVec4(0.85f, 0.85f, 0.9f, 1.0f), "%s", msg.c_str());

		// Barra de progreso (animacion suave: el valor mostrado se acerca al
		// objetivo de forma exponencial en lugar de saltar entre pasos de carga)
		float targetProgress = scene->getLoadProgress();
		float dt = ImGui::GetIO().DeltaTime;
		float ease = 1.0f - std::exp(-6.0f * dt);
		loadDisplayProgress += (targetProgress - loadDisplayProgress) * ease;
		// Cuando ya esta practicamente al nivel objetivo lo fijamos exactamente
		// (evita el "cabeceo" exponencial que nunca llega al valor exacto)
		if (std::fabs(targetProgress - loadDisplayProgress) < 0.005f) {
			loadDisplayProgress = targetProgress;
		}
		float progress = loadDisplayProgress;

		ImVec2 barSize(300.0f, 12.0f);
		ImGui::SetCursorPos(ImVec2(cx - barSize.x * 0.5f, cy + 70.0f));
		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(1.0f, 0.72f, 0.3f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.12f, 0.12f, 0.16f, 1.0f));
		char overlay[32];
		snprintf(overlay, sizeof(overlay), "%d%%", (int)(progress * 100.0f));
		ImGui::ProgressBar(progress, barSize, overlay);
		ImGui::PopStyleColor(2);
	}
	ImGui::End();

	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);
}

//
// FUNCION: GEApplication::draw()
//
/**
 * @brief FunciÃ³n GEApplication::draw
 */
void GEApplication::draw(double fixedDt, int physicsSteps, double alpha, double frameTime)
{
	// 1. SINCRONIZACION
	if (!dc->waitForNextImage(gc.get())) {
		return;
	}
	uint32_t i = dc->getCurrentImage();
	VkCommandBuffer cb = cc->commandBuffers[i];

	int steps = 1;
	scene->updatePhysics(gc.get(), i, frameTime);

	// =========================================================================
	// NUEVO: LEER EL TIEMPO DE LA GPU DEL FOTOGRAMA ANTERIOR
	// =========================================================================
	static float computeShaderTimeMs = 0.0f; // Static para que mantenga el valor entre frames
	static bool isFirstFrame = true;

	if (!isFirstFrame) {
		uint64_t timeStamps[2] = { 0, 0 };

		VkResult result = vkGetQueryPoolResults(
			gc->device, queryPool, 0, 2,
			sizeof(timeStamps), timeStamps, sizeof(uint64_t),
			VK_QUERY_RESULT_64_BIT
		);

		if (result == VK_SUCCESS) {
			VkPhysicalDeviceProperties props;
			vkGetPhysicalDeviceProperties(gc->physicalDevice, &props);

			uint64_t timeElapsedTicks = timeStamps[1] - timeStamps[0];
			float tick_en_nanosegundos = props.limits.timestampPeriod;

			computeShaderTimeMs = (timeElapsedTicks * tick_en_nanosegundos) / 1000000.0f;
		}
	}
	else 
		isFirstFrame = false;
	


	// 2. RESET Y BEGIN 
	// Esperar a que la GPU termine con este frame

	vkResetCommandBuffer(cb, 0);
	VkCommandBufferBeginInfo beginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	if (vkBeginCommandBuffer(cb, &beginInfo) != VK_SUCCESS) return;

	// 3. PREPARAR IMGUI 
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// -----------------------Dibujamos la UI con ImGui-----------------------
	float windowWidth = (float)dc->getExtent().width;
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(windowWidth, 40.0f));

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.9f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	window_flags |= ImGuiWindowFlags_NoScrollbar;

	if (ImGui::Begin("TopBar", nullptr, window_flags))
	{
		ImGui::SetCursorPosY(10.0f);

		// MOSTRAR FPS DE LA APLICACION (CPU)
		ImGui::Text("Rendimiento: %.1f FPS (%.3f ms)",
			ImGui::GetIO().Framerate,
			1000.0f / ImGui::GetIO().Framerate);

		ImGui::SameLine();
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
		ImGui::SameLine();


		
		ImGui::SameLine();
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
		ImGui::SameLine();
		// =====================================================================
		// MOSTRAR TIEMPO DE LA GPU (PARTICULAS)
		// =====================================================================
		ImGui::Text("Particulas GPU: %.4f ms", computeShaderTimeMs);

		ImGui::SameLine();
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
		ImGui::SameLine();
		// =====================================================================
		ImGui::Text("nParticulas: %u", scene->getTotalParticleCount()); // %u es para imprimir uint32_t

		ImGui::SameLine();
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
		ImGui::SameLine();

		auto camera = scene->getCamera();

		switch (camera->getCurrentMode()) {
		/**
		 * @brief FunciÃ³n ImGui::Text
		 */
		case GECamera::CameraMode::FREE:         ImGui::Text("Modo: Libre"); break;
		/**
		 * @brief FunciÃ³n ImGui::Text
		 */
		case GECamera::CameraMode::FPS:          ImGui::Text("Modo: FPS"); break;
		/**
		 * @brief FunciÃ³n ImGui::Text
		 */
		case GECamera::CameraMode::OBSERVING:	 ImGui::Text("Observando: %s", camera->getCurrentObservationName().c_str()); break;
		}

		ImGui::End();
	}


	ControlsGUI(scene->getCamera());

	ImGui::PopStyleVar();
	ImGui::PopStyleColor();
	// -----------------------------------------------------------------------

	ImGui::Render();

	// 4. ACTUALIZAR Y DIBUJAR ESCENA
	scene->update(gc.get(), i, alpha);
    scene->recordComputeCommands(cb, i, queryPool, steps); 

	auto rc = scene->getRenderingContext();
	rc->insertBeginCommands(cb, i);

	scene->drawGraphicsObjects(cb, i);

	// 5. DIBUJAR IMGUI 
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
// FUNCION: GEApplication::cleanup()
//
// PROPOSITO: Libera los recursos y finaliza la aplicacion
//
/**
 * @brief FunciÃ³n GEApplication::cleanup
 */
void GEApplication::cleanup()
{
	// Si el hilo de carga sigue activo, esperamos a que termine antes de destruir recursos
	if (loaderThread.joinable()) loaderThread.join();

	if(gc) vkDeviceWaitIdle(gc->device); // ESPERA A LA GPU ANTES DE DESTRUIR NADA

	vkDestroyQueryPool(gc->device, queryPool, nullptr);

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
// FUNCION: GEApplication::swapFullScreen()
//
// PROPOSITO: Dibuja la ventana a pantalla completa o a tamao configurable
//
/**
 * @brief FunciÃ³n GEApplication::swapFullScreen
 */
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
		glfwSetWindowMonitor(window.get(), nullptr, windowPos.Xpos, windowPos.Ypos, windowPos.width, windowPos.height, 0);
	}
}

//
// FUNCION: GEApplication::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
//
// PROPOSITO: Respuesta a un evento de teclado sobre la aplicacion
//
/**
 * @brief FunciÃ³n GEApplication::keyCallback
 */
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
// FUNCION: GEApplication::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
//
// PROPOSITO: Respuesta a un evento de raton sobre la aplicacion
//
/**
 * @brief FunciÃ³n GEApplication::mouseButtonCallback
 */
void GEApplication::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	GEApplication* app = (GEApplication*)glfwGetWindowUserPointer(window);
	if (app->scene) {
		app->scene->mouse_button_action(window, button, action);
	}
}

//
// FUNCION: GEApplication::cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
//
// PROPOSITO: Respuesta a un evento de movimiento del cursor sobre la aplicacion
//
/**
 * @brief FunciÃ³n GEApplication::cursorPositionCallback
 */
void GEApplication::cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
	GEApplication* app = (GEApplication*)glfwGetWindowUserPointer(window);
	if (app->scene) {
		app->scene->mouse_action(xpos, ypos);
	}
}

//
// FUNCION: GEApplication::framebufferResizeCallback(GLFWwindow* window, int width, int height)
//
// PROPOSITO: Respuesta a un evento de redimensionamiento de la ventana de la aplicacion
//
/**
 * @brief FunciÃ³n GEApplication::framebufferResizeCallback
 */
void GEApplication::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	GEApplication* app = (GEApplication*)glfwGetWindowUserPointer(window);
	if (height != 0)
	{
		app->resize();
	}
}

//
// FUNCION: GEApplication::resize()
//
// PROPOSITO: Reconstruye los objetos con el nuevo tamao de ventana
//
/**
 * @brief FunciÃ³n GEApplication::resize
 */
void GEApplication::resize()
{
	// Si la escena se esta cargando en segundo plano, posponemos el resize:
	// no podemos recrear el contexto de render mientras el hilo de carga lo usa.
	if (scene && scene->isLoading()) {
		if (!windowPos.fullScreen)
		{
			glfwGetWindowSize(window.get(), &windowPos.width, &windowPos.height);
			glfwGetWindowPos(window.get(), &windowPos.Xpos, &windowPos.Ypos);
		}
		pendingResize = true;
		return;
	}

	pendingResize = false;

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

/**
 * @brief Función GEApplication::inicializarImGui
 */
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

	// 2. Crear Pool de Descriptores con verificacion
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

	// 3.5 Configurar las fuentes con alto oversampling para que se vean mas suaves
	ImFontConfig uiCfg;
	uiCfg.OversampleH = 4;
	uiCfg.OversampleV = 4;
	io.Fonts->AddFontDefault(&uiCfg);

	// Fuente grande para el titulo de la pantalla de carga (rasterizada a su tamano real)
	ImFontConfig titleCfg;
	titleCfg.SizePixels = 72.0f;
	titleCfg.OversampleH = 4;
	titleCfg.OversampleV = 4;
	bigTitleFont = io.Fonts->AddFontDefault(&titleCfg);

	// 4. Inicializar Binding de GLFW
	if (!ImGui_ImplGlfw_InitForVulkan(this->window.get(), true)) {
		std::cerr << "[ImGui] ERROR: Fallo la inicializacion de ImGui para GLFW." << std::endl;
		return;
	}

	// 5. Configurar informacion de inicializacion para Vulkan
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = gc->instance;
	init_info.PhysicalDevice = gc->physicalDevice;
	init_info.Device = gc->device;
	init_info.QueueFamily = gc->graphicsQueueFamilyIndex;

	// Obtenemos la cola de graficos directamente
	vkGetDeviceQueue(gc->device, gc->graphicsQueueFamilyIndex, 0, &init_info.Queue);
	if (init_info.Queue == VK_NULL_HANDLE) {
		std::cerr << "[ImGui] ERROR: No se pudo obtener la cola (Queue) de Vulkan." << std::endl;
		return;
	}

	init_info.DescriptorPool = this->imguiPool;
	init_info.MinImageCount = 2;
	init_info.ImageCount = renderContext->getImageCount();

	// Configuracion del Pipeline
	init_info.RenderPass = renderContext->getRenderPass();
	init_info.Subpass = 0;
	init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

	// 6. Inicializar Binding de Vulkan
	if (!ImGui_ImplVulkan_Init(&init_info)) {
		std::cerr << "[ImGui] ERROR: Fallo la inicializacion de ImGui para Vulkan." << std::endl;
		return;
	}

	// MENSAJE DE EXITO
	std::cout << "--------------------------------------------------" << std::endl;
	std::cout << "[SUCCESS] ImGui inicializado correctamente!" << std::endl;
	std::cout << "[INFO] RenderPass utilizado: " << renderContext->getRenderPass() << std::endl;
	std::cout << "[INFO] Imagenes en Swapchain: " << renderContext->getImageCount() << std::endl;
	std::cout << "--------------------------------------------------" << std::endl;
}


/**
 * @brief Función GEApplication::ControlsGUI
 */
void GEApplication::ControlsGUI(GECamera* cam)
{
	if (cam == nullptr) return;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	const float MARGEN = 15.0f;

	ImVec2 posicionEsqInfIzq = ImVec2(
		viewport->WorkPos.x + MARGEN,
		viewport->WorkPos.y + viewport->WorkSize.y - MARGEN
	);

	ImGui::SetNextWindowPos(posicionEsqInfIzq, ImGuiCond_Always, ImVec2(0.0f, 1.0f));

	// AADIMOS ImGuiWindowFlags_NoTitleBar para quitar la barra superior por defecto
	ImGuiWindowFlags flags =
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoTitleBar;

	// Variable estatica para recordar si el panel esta abierto o cerrado
	static bool mostrarControles = true;

	ImGui::Begin("Controles de Camara", nullptr, flags);

	// 1. DIBUJAMOS EL CONTENIDO PRIMERO (Quedara en la parte de arriba)
	if (mostrarControles)
	{
		ImGui::Text("Modo actual: ");
		ImGui::SameLine();

		auto mode = cam->getCurrentMode();

		if (mode == GECamera::CameraMode::FREE) {
			ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "LIBRE (Normal)");
			ImGui::Separator();
			ImGui::Text("M: Cambiar modo de camara");
			ImGui::Text("Flechas: Rotar camara (Pitch/Yaw)");
			ImGui::Text("K / L: Rotar Izquierda / Derecha (Roll)");
			ImGui::Text("Q / A: Mover Arriba / Abajo");
			ImGui::Text("O / P: Mover Izquierda / Derecha");
			ImGui::Text("1 / 2: Aumentar / Reducir velocidad de avance");
			ImGui::Text("S: Detener movimiento (Velocidad 0)");
		}
		else if (mode == GECamera::CameraMode::FPS) {
			ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.2f, 1.0f), "FPS");
			ImGui::Separator();
			ImGui::Text("M: Cambiar modo de camara");
			ImGui::Text("W / S: Avanzar / Retroceder");
			ImGui::Text("A / D: Mover Izquierda / Derecha");
			ImGui::Text("Espacio: Subir (Ascender)");
			ImGui::Text("Shift: Bajar (Descender)");
			ImGui::Text("Raton: Mirar alrededor");
		}
		else if (mode == GECamera::CameraMode::OBSERVING) {
			ImGui::TextColored(ImVec4(0.2f, 0.6f, 1.0f, 1.0f), "OBSERVACION");
			ImGui::Separator();
			ImGui::Text("M: Cambiar modo de camara");
			ImGui::Text("Izquierda / Derecha: Cambiar objetivo");
			ImGui::Text("Arriba / Abajo: Alejar / Acercar camara");
			ImGui::Text("Raton: Orbitar alrededor del objetivo");
		}

		ImGui::Separator(); // Una linea visual para separar el contenido del boton inferior
	}

	// 2. DIBUJAMOS EL "TITULO/BOTON" AL FINAL (Quedara en la base)
	// Cambiamos el texto del boton dependiendo de si esta abierto o cerrado
	const char* textoBoton = mostrarControles ? "v  Ocultar Controles de Camara  v" : "^  Mostrar Controles de Camara  ^";

	// Usamos un boton que ocupe todo el ancho disponible
	if (ImGui::Button(textoBoton, ImVec2(ImGui::GetContentRegionAvail().x, 0)))
	{
		mostrarControles = !mostrarControles; // Invertimos el estado al hacer clic
	}

	ImGui::End();
}
