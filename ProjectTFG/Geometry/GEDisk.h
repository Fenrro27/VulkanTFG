/**
 * @file GEDisk.h
 * @brief Archivo GEDisk.h
 */
#pragma once

#include "GEFigure.h"

//
// CLASE: GEDisk
//
// DESCRIPCIÓN: Clase que describe un disco
//
/**
 * @class GEDisk
 * @brief Class GEDisk
 */
class GEDisk : public GEFigure
{
public:
	/**
	 * @brief Función GEDisk
	 */
	explicit GEDisk(int p, int m, float r0, float r1);
};

