#include "GERenderingContext.h"

#include "resource.h"
#include <windows.h>
#include "commonDebug.h"

///////////////////////////////////////////////////////////////////////////////////////////
/////                                                                                 /////
/////                              M�todos p�blicos                                   /////
/////                                                                                 /////
///////////////////////////////////////////////////////////////////////////////////////////

//
// FUNCI�N: GERenderingContext::GERenderingContext()
//
// PROP�SITO: Crea un contexto de renderizado (pipelines, renderpass y framebuffer) 
//
GERenderingContext::GERenderingContext(GEGraphicsContext* gc, GEDrawingContext* dc, GEPipelineConfig* config)
{
	imageCount = dc->getImageCount();
	format = dc->getFormat();
	extent = dc->getExtent();
	pipelineIndex = 0;
	graphicsPipeline.resize(0);
	descriptorSetLayout.resize(0);
	pipelineLayout.resize(0);
	createRenderPass(gc);
	createGraphicsPipeline(gc, config);
	createDepthBuffers(gc);
	createFramebuffers(gc, dc);
	viewport.x = 0.0f;
	this->pipelineIndex = 0;
}

//
// FUNCI�N: GERenderingContext::destroy(GEGraphicsContext* gc)
//
// PROP�SITO: Crea el estado de Vulkan
//
void GERenderingContext::destroy(GEGraphicsContext* gc)
{
	for (uint32_t i = 0; i < imageCount; i++)
	{
		vkDestroyFramebuffer(gc->device, framebuffers[i], nullptr);
		depthBuffers[i]->destroy(gc);
	}
	for (uint32_t i = 0; i < graphicsPipeline.size(); i++)
	{
		vkDestroyPipeline(gc->device, graphicsPipeline[i], nullptr);
		vkDestroyPipelineLayout(gc->device, pipelineLayout[i], nullptr);
		vkDestroyDescriptorSetLayout(gc->device, descriptorSetLayout[i], nullptr);
	}
	vkDestroyRenderPass(gc->device, renderPass, nullptr);
}

//
// FUNCI�N: GERenderingContext::createGraphicsPipeline()
//
// PROP�SITO: A�ade un nuevo Pipeline de renderizado
//
void GERenderingContext::addGraphicsPipeline(GEGraphicsContext* gc, GEPipelineConfig* config)
{
	pipelineIndex = graphicsPipeline.size();
	createGraphicsPipeline(gc, config);
}

//
// FUNCI�N: GERenderingContext::setActivePipeline(uint32_t index)
//
// PROP�SITO: Asigna el pipeline activo
//
void GERenderingContext::setActivePipeline(uint32_t index)
{
	if (index < graphicsPipeline.size()) pipelineIndex = index;
	GE_DEBUG_INFO("setActivePipeline: Pipeline activo cambiado a: " << pipelineIndex);
}

//
// FUNCI�N: GERenderingContext::getActiveDescriptorSetLayout()
//
// PROP�SITO: Obtiene la plantilla de descriptor set del pipeline activo
//
VkDescriptorSetLayout GERenderingContext::getActiveDescriptorSetLayout()
{
	return descriptorSetLayout[pipelineIndex];
}

//
// FUNCI�N: GERenderingContext::getActivePipelineLayout()
//
// PROP�SITO: Obtiene la plantilla de pipeline del pipeline activo
//
VkPipelineLayout GERenderingContext::getActivePipelineLayout()
{
	return pipelineLayout[pipelineIndex];
}

//
// FUNCI�N: GERenderingContext::insertBeginCommands(std::vector<VkCommandBuffer> commandBuffers)
//
// PROP�SITO: A�ade la inicializaci�n del renderpass a los buffers de comandos
//
void GERenderingContext::insertBeginCommands(VkCommandBuffer cb, uint32_t index)
{
	
		VkClearValue clearValues[2];
		clearValues[0].color = { 1.0f, 1.0f, 1.0f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass;
		renderPassInfo.framebuffer = framebuffers[index];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = extent;
		renderPassInfo.clearValueCount = 2;
		renderPassInfo.pClearValues = clearValues;

		vkCmdBeginRenderPass(cb, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	
}

//
// FUNCI�N: GERenderingContext::insertPipelineCommands(std::vector<VkCommandBuffer> commandBuffers)
//
// PROP�SITO: A�ade la selecci�n de pipeline a los buffers de comandos
//
void GERenderingContext::insertPipelineCommands(VkCommandBuffer cb, uint32_t index)
{
		vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline[index]);
}

//
// FUNCI�N: GERenderingContext::insertEndCommands(std::vector<VkCommandBuffer> commandBuffers)
//
// PROP�SITO: A�ade la finalizaci�n del renderpass a los buffers de comandos
//
void GERenderingContext::insertEndCommands(VkCommandBuffer cb)
{
		vkCmdEndRenderPass(cb);
}

///////////////////////////////////////////////////////////////////////////////////////////
/////                                                                                 /////
/////                    M�todos de creaci�n de los componentes                       /////
/////                                                                                 /////
///////////////////////////////////////////////////////////////////////////////////////////

//
// FUNCI�N: GERenderingContext::createRenderPass()
//
// PROP�SITO: Crea los pasos de renderizado
//
void GERenderingContext::createRenderPass(GEGraphicsContext* gc)
{
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = format;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentDescription depthAttachment = {};
	depthAttachment.format = gc->findDepthFormat();
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef{};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	VkAttachmentDescription attachment[] = { colorAttachment, depthAttachment };

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 2;
	renderPassInfo.pAttachments = attachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(gc->device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create render pass!");
	}
}

//
// FUNCI�N: GERenderingContext::createGraphicsPipeline()
//
// PROP�SITO: Crea el Pipeline de renderizado
//
void GERenderingContext::createGraphicsPipeline(GEGraphicsContext* gc, GEPipelineConfig* config)
{
	
	viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)config->extent.width;
	viewport.height = (float)config->extent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = config->extent;

	VkShaderModule vertShaderModule;
	VkShaderModule fragShaderModule;
	VkPipelineShaderStageCreateInfo vertShaderStageInfo;
	VkPipelineShaderStageCreateInfo fragShaderStageInfo;
	VkPipelineVertexInputStateCreateInfo vertexInputInfo;
	VkPipelineInputAssemblyStateCreateInfo inputAssembly;
	VkPipelineViewportStateCreateInfo viewportState;
	VkPipelineRasterizationStateCreateInfo rasterizer;
	VkPipelineMultisampleStateCreateInfo multisampling;
	VkPipelineDepthStencilStateCreateInfo depthStencil;
	VkPipelineColorBlendAttachmentState colorBlendAttachment;
	VkPipelineColorBlendStateCreateInfo colorBlending;

	createPipelineLayout(gc, config);
	createVertexShaderStageCreateInfo(gc, config->vertex_shader, &vertShaderModule, &vertShaderStageInfo);
	createFragmentShaderStageCreateInfo(gc, config->fragment_shader, &fragShaderModule, &fragShaderStageInfo);
	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };
	createPipelineVertexInputStateCreateInfo(config, &vertexInputInfo);
	createPipelineInputAssemblyStateCreateInfo(config, &inputAssembly);
	createPipelineViewportStateCreateInfo(&viewportState);
	createPipelineRasterizationStateCreateInfo(config, &rasterizer);
	createPipelineMultisampleStateCreateInfo(&multisampling);
	createPipelineDepthStencilStateCreateInfo(config, &depthStencil);
	createPipelineColorBlendStateCreateInfo(&colorBlendAttachment, &colorBlending);

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = &depthStencil;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = nullptr;
	pipelineInfo.layout = pipelineLayout[pipelineIndex];
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;


	GE_DEBUG_INFO( "Creando Pipeline Indice: " << pipelineIndex <<
		"\n" << "\t- Vertex Shader ID: " << config->vertex_shader <<
		"\n" << "\t- Fragment Shader ID: " << config->fragment_shader <<
		"\n" << "\t- Usando Layout: " << pipelineLayout[pipelineIndex]);


	VkPipeline pipeline;
	if (vkCreateGraphicsPipelines(gc->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create graphics pipeline!");
	}

	graphicsPipeline.push_back(pipeline);

	vkDestroyShaderModule(gc->device, fragShaderModule, nullptr);
	vkDestroyShaderModule(gc->device, vertShaderModule, nullptr);
}

//
// FUNCI�N: GERenderingContext::createDepthBuffers()
//
// PROP�SITO: Crea los buffers de profundidad
//
void GERenderingContext::createDepthBuffers(GEGraphicsContext* gc)
{
	depthBuffers.resize(imageCount);
	for (size_t i = 0; i < imageCount; i++)
	{
		depthBuffers[i] = new GEDepthBuffer(gc, extent);
	}
}

//
// FUNCI�N: GERenderingContext::createFramebuffers()
//
// PROP�SITO: Crea un Framebuffer para cada imagen del swapchain
//
void GERenderingContext::createFramebuffers(GEGraphicsContext* gc,
	GEDrawingContext* dc)
{
	framebuffers.resize(imageCount);

	for (size_t i = 0; i < imageCount; i++)
	{
		VkImageView attachments[] = { dc->imageViews[i], depthBuffers[i]->imageView };

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = 2;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = extent.width;
		framebufferInfo.height = extent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(gc->device, &framebufferInfo, nullptr, &framebuffers[i])
			!= VK_SUCCESS)
		{
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////
/////                                                                                 /////
/////              M�todos de definici�n del pipeline de renderizado                  /////
/////                                                                                 /////
///////////////////////////////////////////////////////////////////////////////////////////

//
// FUNCI�N: CAVulkanState::createPipelineLayout()
//
// PROP�SITO: Crea el esquema de los conjuntos de descriptores
//
void GERenderingContext::createPipelineLayout(GEGraphicsContext* gc, GEPipelineConfig* config)
{
	uint32_t bindingCount = (uint32_t)config->descriptorTypes.size();
	std::vector<VkDescriptorSetLayoutBinding> bindings(bindingCount);
	for (uint32_t i = 0; i < bindingCount; i++)
	{
		VkDescriptorSetLayoutBinding binding = {};
		binding.binding = i;
		binding.descriptorCount = 1;
		binding.descriptorType = config->descriptorTypes[i];
		binding.pImmutableSamplers = nullptr;
		binding.stageFlags = config->descriptorStages[i];

		bindings[i] = binding;
	}

	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = bindingCount;
	layoutInfo.pBindings = bindings.data();

	VkDescriptorSetLayout dsetLayout;
	if (vkCreateDescriptorSetLayout(gc->device, &layoutInfo, nullptr, &dsetLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor set layout!");
	}
	descriptorSetLayout.push_back(dsetLayout);

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout[pipelineIndex];
	pipelineLayoutInfo.pushConstantRangeCount = 0;

	VkPipelineLayout pipLayout;
	if (vkCreatePipelineLayout(gc->device, &pipelineLayoutInfo, nullptr, &pipLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create pipeline layout!");
	}

	pipelineLayout.push_back(pipLayout);
}

//
// FUNCI�N: GERenderingContext::createVertexShaderStageCreateInfo()
//
// PROP�SITO: Crea la informaci�n sobre el Vertex Shader
//
void GERenderingContext::createVertexShaderStageCreateInfo(GEGraphicsContext* gc, int resource, VkShaderModule* vertShaderModule, VkPipelineShaderStageCreateInfo* vertShaderStageInfo)
{
	std::vector<char> vertShaderCode = getFileFromResource(resource);

	*vertShaderModule = createShaderModule(gc, vertShaderCode);

	*vertShaderStageInfo = {};
	vertShaderStageInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo->stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo->module = *vertShaderModule;
	vertShaderStageInfo->pName = "main";
}

//
// FUNCI�N: GERenderingContext::createFragmentShaderStageCreateInfo()
//
// PROP�SITO: Crea la informaci�n sobre el Vertex Shader
//
void GERenderingContext::createFragmentShaderStageCreateInfo(GEGraphicsContext* gc, int resource, VkShaderModule* fragShaderModule, VkPipelineShaderStageCreateInfo* fragShaderStageInfo)
{
	std::vector<char> fragShaderCode = getFileFromResource(resource);

	*fragShaderModule = createShaderModule(gc, fragShaderCode);

	*fragShaderStageInfo = {};
	fragShaderStageInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo->stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo->module = *fragShaderModule;
	fragShaderStageInfo->pName = "main";
}

//
// FUNCI�N: GERenderingContext::createPipelineVertexInputStateCreateInfo()
//
// PROP�SITO: Crea la descripci�n de los atributos de los v�rtices
//
void GERenderingContext::createPipelineVertexInputStateCreateInfo(const GEPipelineConfig* config, VkPipelineVertexInputStateCreateInfo* vertexInputInfo)
{
	
	int attrCount = (int)config->attrOffsets.size();
	int bindingCount = (attrCount > 0 ? 1 : 0);

	bindingDescriptions.resize(bindingCount);
	for (int i = 0; i < bindingCount; i++)
	{
		bindingDescriptions[i] = {};
		bindingDescriptions[i].binding = i;
		bindingDescriptions[i].stride = config->attrStride;
		bindingDescriptions[i].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	}

	attributeDescriptions.resize(attrCount);
	for (int i = 0; i < attrCount; i++)
	{
		attributeDescriptions[i] = {};
		attributeDescriptions[i].binding = 0;
		attributeDescriptions[i].location = i;
		attributeDescriptions[i].format = config->attrFormats[i];
		attributeDescriptions[i].offset = config->attrOffsets[i];
	}

	*vertexInputInfo = {};
	vertexInputInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo->vertexBindingDescriptionCount = bindingCount;
	vertexInputInfo->vertexAttributeDescriptionCount = attrCount;
	vertexInputInfo->pVertexBindingDescriptions = bindingDescriptions.data();
	vertexInputInfo->pVertexAttributeDescriptions = attributeDescriptions.data();
}

//
// FUNCI�N: GERenderingContext::createPipelineVertexInputStateCreateInfo()
//
// PROP�SITO: Crea la informaci�n para el ensamblado de primitivas
//
void GERenderingContext::createPipelineInputAssemblyStateCreateInfo(const GEPipelineConfig* config, VkPipelineInputAssemblyStateCreateInfo* inputAssembly)
{
	*inputAssembly = {};
	inputAssembly->sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly->topology = config->topology;
	inputAssembly->primitiveRestartEnable = VK_FALSE;
}

//
// FUNCI�N: CAVulkanState::createPipelineViewportStateCreateInfo()
//
// PROP�SITO: Crea la informaci�n del viewport
//
void GERenderingContext::createPipelineViewportStateCreateInfo(VkPipelineViewportStateCreateInfo* viewportState)
{
	*viewportState = {};
	viewportState->sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState->viewportCount = 1;
	viewportState->pViewports = &viewport;
	viewportState->scissorCount = 1;
	viewportState->pScissors = &scissor;
}

//
// FUNCI�N: GERenderingContext::createPipelineRasterizationStateCreateInfo()
//
// PROP�SITO: Crea la informaci�n de la etapa de rasterizaci�n
//
void GERenderingContext::createPipelineRasterizationStateCreateInfo(const GEPipelineConfig* config, VkPipelineRasterizationStateCreateInfo* rasterizer)
{
	*rasterizer = {};
	rasterizer->sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer->depthClampEnable = VK_FALSE;
	rasterizer->rasterizerDiscardEnable = VK_FALSE;
	rasterizer->polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer->lineWidth = 1.0f;
	rasterizer->cullMode = config->cullMode;
	rasterizer->frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer->depthBiasEnable = VK_FALSE;
}

//
// FUNCI�N: GERenderingContext::createPipelineRasterizationStateCreateInfo()
//
// PROP�SITO: Crea la informaci�n de la etapa de sampleado
//
void GERenderingContext::createPipelineMultisampleStateCreateInfo(VkPipelineMultisampleStateCreateInfo* multisampling)
{
	*multisampling = {};
	multisampling->sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling->sampleShadingEnable = VK_FALSE;
	multisampling->rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
}

//
// FUNCI�N: GERenderingContext::createPipelineRasterizationStateCreateInfo()
//
// PROP�SITO: Crea la informaci�n sobre los tests de profundidad y de plantilla
//
void GERenderingContext::createPipelineDepthStencilStateCreateInfo(const GEPipelineConfig* config, VkPipelineDepthStencilStateCreateInfo* depthStencil)
{
	*depthStencil = {};
	depthStencil->sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil->pNext = nullptr;
	depthStencil->flags = 0;
	depthStencil->depthTestEnable = config->depthTestEnable;
	depthStencil->depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	depthStencil->depthWriteEnable = config->depthWrite;
	depthStencil->depthBoundsTestEnable = VK_FALSE;
	depthStencil->stencilTestEnable = VK_FALSE;
}

//
// FUNCI�N: GERenderingContext::createPipelineColorBlendStateCreateInfo()
//
// PROP�SITO: Crea la informaci�n sobre la etapa de mezcla de colores
//
void GERenderingContext::createPipelineColorBlendStateCreateInfo(VkPipelineColorBlendAttachmentState* colorBlendAttachment, VkPipelineColorBlendStateCreateInfo* colorBlending)
{
	*colorBlendAttachment = {};
	colorBlendAttachment->colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment->blendEnable = VK_TRUE;

	colorBlendAttachment->srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachment->dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachment->colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment->srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment->dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment->alphaBlendOp = VK_BLEND_OP_ADD;

	*colorBlending = {};
	colorBlending->sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending->logicOpEnable = VK_FALSE;
	colorBlending->logicOp = VK_LOGIC_OP_COPY;
	colorBlending->attachmentCount = 1;
	colorBlending->pAttachments = colorBlendAttachment;
	colorBlending->blendConstants[0] = 0.0f;
	colorBlending->blendConstants[1] = 0.0f;
	colorBlending->blendConstants[2] = 0.0f;
	colorBlending->blendConstants[3] = 0.0f;
}

///////////////////////////////////////////////////////////////////////////////////////////
/////                                                                                 /////
/////                              M�todos auxiliares                                 /////
/////                                                                                 /////
///////////////////////////////////////////////////////////////////////////////////////////

//
// FUNCI�N: GERenderingContext::createShaderModule()
//
// PROP�SITO: Crea un shader a partir de su c�digo en SPIR-V
//
VkShaderModule GERenderingContext::createShaderModule(GEGraphicsContext* gc, const std::vector<char>& code)
{
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(gc->device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create shader module!");
	}

	return shaderModule;
}

//
// FUNCI�N: GERenderingContext::getFileFromResource(int resource)
//
// PROP�SITO: Extrae el contenido de un fichero incluido como recurso de la aplicaci�n
//
std::vector<char> GERenderingContext::getFileFromResource(int resource)
{
	HRSRC shaderHandle = FindResource(nullptr, MAKEINTRESOURCE(resource), RT_HTML);
	HGLOBAL shaderGlobal = LoadResource(nullptr, shaderHandle);
	LPCTSTR shaderPtr = static_cast<LPCTSTR>(LockResource(shaderGlobal));
	DWORD shaderSize = SizeofResource(nullptr, shaderHandle);

	std::vector<char> shader(shaderSize);
	memcpy(shader.data(), shaderPtr, shaderSize);
	UnlockResource(shaderGlobal);
	FreeResource(shaderGlobal);
	return shader;
}