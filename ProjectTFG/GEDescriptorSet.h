/**
 * @file GEDescriptorSet.h
 * @brief Definición de la clase GEDescriptorSet para la gestión de Descriptor Sets en Vulkan.
 */
#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "GEGraphicsContext.h"
#include "GERenderingContext.h"
#include "GEUniformBuffer.h"
#include "GETexture.h"


class GERenderingContext;

/**
 * @class GEDescriptorSet
 * @brief Clase que encapsula la creación y gestión de conjuntos de descriptores (Descriptor Sets).
 * 
 * Los conjuntos de descriptores se utilizan para vincular recursos como Uniform Buffers 
 * y Texturas a los shaders.
 */
class GEDescriptorSet
{
private:
	VkDescriptorPool descriptorPool; /**< Pool de descriptores desde donde se asignan los sets. */

public:
	std::vector<VkDescriptorSet> descriptorSets; /**< Lista de sets de descriptores asignados. */

public:
	/**
	 * @brief Constructor de GEDescriptorSet.
	 * @param gc Contexto de gráficos.
	 * @param rc Contexto de renderizado.
	 * @param ubos Lista de punteros a Uniform Buffers.
	 * @param tex Lista de punteros a Texturas.
	 */
	GEDescriptorSet(GEGraphicsContext* gc, GERenderingContext* rc, std::vector<GEUniformBuffer*> ubos, std::vector<GETexture*> tex);

	/**
	 * @brief Libera los recursos asociados al pool de descriptores.
	 * @param gc Contexto de gráficos.
	 */
	void destroy(GEGraphicsContext* gc);
};
