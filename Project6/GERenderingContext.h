/**
 * @file GERenderingContext.h
 * @brief Definición de la clase GERenderingContext para la gestión del pipeline de renderizado y Render Pass.
 */
#pragma once

#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>
#include <memory>
#include "GEGraphicsContext.h"
#include "GEDrawingContext.h"
#include "GEPipelineConfig.h"
#include "GEDepthBuffer.h"

/**
 * @class GERenderingContext
 * @brief Clase que encapsula el estado de renderizado de Vulkan.
 * 
 * Gestiona el Render Pass, los pipelines gráficos, los descriptor set layouts,
 * los framebuffers y los depth buffers asociados.
 */
class GERenderingContext
{
public:
	uint32_t imageCount; /**< Número de imágenes en la swapchain. */

private:
	uint32_t pipelineIndex; /**< Índice del pipeline activo. */
	VkFormat format; /**< Formato de imagen. */
	VkExtent2D extent; /**< Dimensiones del renderizado. */
	VkRenderPass renderPass; /**< Objeto Render Pass de Vulkan. */
	std::vector<VkPipeline> graphicsPipeline; /**< Lista de pipelines gráficos creados. */
	std::vector<VkDescriptorSetLayout> descriptorSetLayout; /**< Layouts de los conjuntos de descriptores. */
	std::vector<VkPipelineLayout> pipelineLayout; /**< Layouts de los pipelines. */
	std::vector<std::unique_ptr<GEDepthBuffer>> depthBuffers; /**< Buffers de profundidad por cada imagen. */
	std::vector<VkFramebuffer> framebuffers; /**< Framebuffers vinculados a las imágenes de la swapchain. */
	VkViewport viewport; /**< Configuración del Viewport. */
	VkRect2D scissor; /**< Configuración del Scissor. */
	
	std::vector<VkVertexInputBindingDescription> bindingDescriptions; /**< Descripciones de vinculación de vértices. */
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions; /**< Descripciones de atributos de vértices. */

public:
	/**
	 * @brief Constructor de GERenderingContext.
	 * @param gc Contexto de gráficos.
	 * @param dc Contexto de dibujado.
	 * @param config Configuración inicial del pipeline.
	 */
	GERenderingContext(GEGraphicsContext* gc, GEDrawingContext* dc, GEPipelineConfig* config);

	/**
	 * @brief Libera todos los recursos de Vulkan asociados al contexto de renderizado.
	 */
	void destroy(GEGraphicsContext* gc);

	/**
	 * @brief Añade un nuevo pipeline gráfico basado en una configuración.
	 */
	void addGraphicsPipeline(GEGraphicsContext* gc, GEPipelineConfig* config);

	/**
	 * @brief Establece el pipeline activo por su índice.
	 */
	void setActivePipeline(uint32_t index);

	/**
	 * @brief Obtiene el layout del descriptor set del pipeline activo.
	 */
	VkDescriptorSetLayout getActiveDescriptorSetLayout();

	/**
	 * @brief Obtiene el layout del pipeline activo.
	 */
	VkPipelineLayout getActivePipelineLayout();

	/**
	 * @brief Inserta los comandos de inicio de un Render Pass.
	 */
	void insertBeginCommands(VkCommandBuffer cb, uint32_t index);

	/**
	 * @brief Inserta los comandos para vincular el pipeline activo.
	 */
	void insertPipelineCommands(VkCommandBuffer cb, uint32_t index);

	/**
	 * @brief Inserta los comandos de fin de un Render Pass.
	 */
	void insertEndCommands(VkCommandBuffer cb);

	/**
	 * @brief Obtiene el objeto VkPipeline activo.
	 */
	VkPipeline getActivePipeline() {
		return graphicsPipeline[pipelineIndex];
	}

	/**
	 * @brief Obtiene el objeto VkRenderPass.
	 */
	VkRenderPass getRenderPass() const { return renderPass; }

	/**
	 * @brief Obtiene la cantidad de imágenes.
	 */
	uint32_t getImageCount() const { return imageCount; }

private:
	/**
	 * @brief Crea el Render Pass.
	 */
	void createRenderPass(GEGraphicsContext* gc);

	/**
	 * @brief Crea un pipeline gráfico.
	 */
	void createGraphicsPipeline(GEGraphicsContext* gc, GEPipelineConfig* config);

	/**
	 * @brief Crea los buffers de profundidad.
	 */
	void createDepthBuffers(GEGraphicsContext* gc);

	/**
	 * @brief Crea los framebuffers vinculándolos a la swapchain.
	 */
	void createFramebuffers(GEGraphicsContext* gc, GEDrawingContext* dc);

	/**
	 * @brief Crea el layout del pipeline.
	 */
	void createPipelineLayout(GEGraphicsContext* gc, GEPipelineConfig* config);

	/**
	 * @brief Crea la información de etapa del shader de vértices.
	 */
	void createVertexShaderStageCreateInfo(GEGraphicsContext* gc, int resource, VkShaderModule* vertShaderModule, VkPipelineShaderStageCreateInfo* vertShaderStageInfo);

	/**
	 * @brief Crea la información de etapa del shader de fragmentos.
	 */
	void createFragmentShaderStageCreateInfo(GEGraphicsContext* gc, int resource, VkShaderModule* fragShaderModule, VkPipelineShaderStageCreateInfo* fragShaderStageInfo);

	/**
	 * @brief Configura el estado de entrada de vértices para el pipeline.
	 */
	void createPipelineVertexInputStateCreateInfo(const GEPipelineConfig* config, VkPipelineVertexInputStateCreateInfo* vertexInputInfo);

	/**
	 * @brief Configura el ensamblado de entrada (topología).
	 */
	void createPipelineInputAssemblyStateCreateInfo(const GEPipelineConfig* config, VkPipelineInputAssemblyStateCreateInfo* inputAssembly);

	/**
	 * @brief Configura el estado del viewport y scissor.
	 */
	void createPipelineViewportStateCreateInfo(VkPipelineViewportStateCreateInfo* viewportState);

	/**
	 * @brief Configura el rasterizador (modo de rellenado, culling, etc.).
	 */
	void createPipelineRasterizationStateCreateInfo(const GEPipelineConfig* config, VkPipelineRasterizationStateCreateInfo* rasterizer);

	/**
	 * @brief Configura el multisampling.
	 */
	void createPipelineMultisampleStateCreateInfo(VkPipelineMultisampleStateCreateInfo* multisampling);

	/**
	 * @brief Configura el estado de profundidad y stencil.
	 */
	void createPipelineDepthStencilStateCreateInfo(const GEPipelineConfig* config, VkPipelineDepthStencilStateCreateInfo* depthStencil);

	/**
	 * @brief Configura el mezclado de colores (blending).
	 */
	void createPipelineColorBlendStateCreateInfo(VkPipelineColorBlendAttachmentState* colorBlendAttachment, VkPipelineColorBlendStateCreateInfo* colorBlending);

	/**
	 * @brief Crea un módulo de shader a partir de código binario (SPIR-V).
	 */
	VkShaderModule createShaderModule(GEGraphicsContext* gc, const std::vector<char>& code);

	/**
	 * @brief Carga los datos de un recurso.
	 */
	std::vector<char> getFileFromResource(int resource);
};
