/**
 * @file GETorus.h
 * @brief Archivo GETorus.h
 */
#pragma once

#include "GEFigure.h"

//
// CLASE: GETorus
//
// DESCRIPCIÓN: Clase que describe un toroide
//
/**
 * @class GETorus
 * @brief Class GETorus
 */
class GETorus : public GEFigure
{
public:
	GETorus(int p, int m, float r0, float r1);
};

