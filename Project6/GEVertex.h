/**
 * @file GEVertex.h
 * @brief Archivo GEVertex.h
 */
#pragma once

#include <glm\glm.hpp>

//
// ESTRUCTURA: GEVertex
//
// DESCRIPCIÓN: Estructura que describe los atributos de un vértice

/**
 * @struct GEVertex
 * @brief Struct GEVertex
 */
struct GEVertex
{
	alignas(16) glm::vec3 pos;
	alignas(16) glm::vec3 norm;
	alignas(8) glm::vec2 tex;

	bool operator==(const GEVertex& other) const {
		return pos == other.pos &&
			norm == other.norm &&
			tex == other.tex;
	}
};

