/**
 * @file GEWindowPosition.h
 * @brief Archivo GEWindowPosition.h
 */
#pragma once

//
// ESTRUCTURA: GEWindowPosition
//
// DESCRIPCIÓN: Estructura que almacena la posición y el tamaño de la ventana gráfica
//
/**
 * @struct GEWindowPosition
 * @brief Struct GEWindowPosition
 */
struct GEWindowPosition {
	int width;
	int height;
	int Xpos;
	int Ypos;
	int screenWidth;
	int screenHeight;
	bool fullScreen;
};
