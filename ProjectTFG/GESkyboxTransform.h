/**
 * @file GESkyboxTransform.h
 * @brief Archivo GESkyboxTransform.h
 */
#pragma once

#include <glm/glm.hpp>

//
// ESTRUCTURA: GESkyboxTransform
//
// DESCRIPCIÓN: Estructura que describe la matricz de transformación de un skybox
//
 /**
  * @struct GESkyboxTransform
  * @brief Struct GESkyboxTransform
  */
 struct GESkyboxTransform
{
	alignas(16) glm::mat4 inverse;
};