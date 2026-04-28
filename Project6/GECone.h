/**
 * @file GECone.h
 * @brief Archivo GECone.h
 */
#pragma once

#include "GEFigure.h"

//
// CLASE: GECone
//
// DESCRIPCIÓN: Clase que describe un cono
//
/**
 * @class GECone
 * @brief Class GECone
 */
class GECone : public GEFigure
{
public:
	GECone(GLint p, GLint m, GLfloat h, GLfloat r);
};

