#pragma once

#include <glm\glm.hpp>

//
// ESTRUCTURA: GEVertex
//
// DESCRIPCIÓN: Estructura que describe los atributos de un vértice

struct GEVertex
{
	alignas(16) glm::vec3 pos;
	alignas(16) glm::vec3 norm;
	alignas(8) glm::vec2 tex;
};

