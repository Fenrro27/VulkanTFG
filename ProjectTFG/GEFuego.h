/**
 * @file GEFuego.h
 * @brief DefiniciÃ³n de la clase GEFuego para la simulaciÃ³n de partÃ­culas de fuego.
 */
#pragma once
#include "GEParticlesSystem.h"

/**
 * @class GEFuego
 * @brief Clase que representa un sistema de partÃ­culas de fuego.
 * 
 * Esta clase hereda de GEParticlesSystem y configura los parÃ¡metros especÃ­ficos
 * para simular el efecto visual del fuego.
 */
class GEFuego : public GEParticlesSystem
{
public:
	/**
	 * @brief Constructor de la clase GEFuego.
	 * @param particleCount NÃºmero inicial de partÃ­culas en el sistema.
	 */
	explicit GEFuego(uint32_t particleCount = 100);
};

