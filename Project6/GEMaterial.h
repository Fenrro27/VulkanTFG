/**
 * @file GEMaterial.h
 * @brief Archivo GEMaterial.h
 */
#pragma once

#include <glm/glm.hpp>

/**
 * @struct GEMaterial
 * @brief Struct GEMaterial
 */
struct GEMaterial
{

	alignas(16) glm::vec3 Ka = glm::vec3(1.0f); // Ambient reflectivity
	alignas(16) glm::vec3 Kd = glm::vec3(1.0f); // Difusse reflectivity
	alignas(16) glm::vec3 Ks = glm::vec3(0.5f); // Specular reflectivity
	alignas(4) float Shininess = 16.0f; // Specular shininess factor
   
};

