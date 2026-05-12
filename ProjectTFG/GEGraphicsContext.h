/**
 * @file GEGraphicsContext.h
 * @brief Archivo GEGraphicsContext.h
 */
#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

//
// CLASE: GEGraphicsContext
//
// DESCRIPCIÓN: Clase que almacena el contexto gráfico de vulkan (instancia y dispositivo)
//
/**
 * @class GEGraphicsContext
 * @brief Class GEGraphicsContext
 */
class GEGraphicsContext
{
public:
	VkInstance instance;
	VkSurfaceKHR surface;
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	uint32_t graphicsQueueFamilyIndex;
	uint32_t presentQueueFamilyIndex;


private:
	VkPhysicalDeviceMemoryProperties memProperties;

public:
	/**
	 * @brief Función GEGraphicsContext
	 */
	explicit GEGraphicsContext(GLFWwindow* window);
	~GEGraphicsContext();
	/**
	 * @brief Función findMemoryType
	 */
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	/**
	 * @brief Función findDepthFormat
	 */
	VkFormat findDepthFormat();

private:
	// Métodos de inicialización de Vulkan
	/**
	 * @brief Función createInstance
	 */
	void createInstance();
	/**
	 * @brief Función createSurface
	 */
	void createSurface(GLFWwindow* window);
	/**
	 * @brief Función pickPhysicalDevice
	 */
	void pickPhysicalDevice();
	/**
	 * @brief Función createLogicalDevice
	 */
	void createLogicalDevice();

	// Métodos auxiliares
	/**
	 * @brief Función showInstanceProperties
	 */
	void showInstanceProperties();
	/**
	 * @brief Función isDeviceSuitable
	 */
	bool isDeviceSuitable(VkPhysicalDevice pDevice);
	/**
	 * @brief Función showDevices
	 */
	void showDevices();
	/**
	 * @brief Función resumeDeviceProperties
	 */
	void resumeDeviceProperties(VkPhysicalDevice pDevice, int index);
};

