#include "GEPipelineConfig.h"


//
// FUNCIÓN: GEPipelineConfig::GEPipelineConfig()
//
// PROPÓSITO: Construye un objeto con los valores por defecto
//
GEPipelineConfig::GEPipelineConfig()
{
	vertex_shader = -1;
	fragment_shader = -1;

	attrStride = 0;
	attrFormats.resize(0);
	attrOffsets.resize(0);

	descriptorTypes.resize(0);
	descriptorStages.resize(0);

	depthTestEnable = VK_TRUE;
	cullMode = VK_CULL_MODE_BACK_BIT;
	extent.width = 800;
	extent.height = 600;

	//compute shader
	topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
}