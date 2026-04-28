/**
 * @file GEFuego.cpp
 * @brief ImplementaciÃ³n de la clase GEFuego para la simulaciÃ³n de partÃ­culas de fuego.
 */
#include "GEFuego.h"
#include <random>

/**
 * @brief Constructor de GEFuego.
 * 
 * Inicializa el sistema de partÃ­culas con parÃ¡metros de fuego, configurando
 * la posiciÃ³n del emisor, colores, tamaÃ±os y la dispersiÃ³n inicial de las partÃ­culas.
 * 
 * @param particleCount Cantidad de partÃ­culas a generar.
 */
GEFuego::GEFuego(uint32_t particleCount) : GEParticlesSystem()
{
	// 1. Usamos 'static' para que el generador se inicialice una sola vez 
	// y no en cada instancia de GEFuego.
	static std::random_device rd;
	/**
	 * @brief Generador de nÃºmeros aleatorios.
	 */
	static std::mt19937 gen(rd());

	// 2. Definimos distribuciones para los rangos especÃ­ficos que necesitas
	/**
	 * @brief DistribuciÃ³n para el tiempo de vida (TTL) de las partÃ­culas.
	 */
	std::uniform_real_distribution<float> distTTL(0.5f, 2.5f);
	/**
	 * @brief DistribuciÃ³n para el porcentaje inicial del TTL.
	 */
	std::uniform_real_distribution<float> distPercent(0.0f, 1.0f);


	emitterParams.particleType = 1; // 1 = Fuego
	emitterParams.emitterPos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f); 
	emitterParams.force = glm::vec4(0.0f, 2.0f, 0.0f, 0.0f); // Impulso hacia arriba
	emitterParams.startColor = glm::vec4(1.0f, 0.8f, 0.1f, 1.0f); // Amarillo
	emitterParams.endColor = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);   // Rojo y transparente
	emitterParams.randomness = 2.0f; // Mucho caos lateral
	emitterParams.startSize = 3.0f;
	emitterParams.endSize = 0.5f; // Las llamas se encogen
	emitterParams.time = 0.0f;
	emitterParams.deltaTime = 0.016f;

	// InicializaciÃ³n de cada partÃ­cula individual
	for (uint32_t i = 0; i < particleCount; i++) {
		GEParticle p;
		p.pos = glm::vec3(0.0f, 0.0f, 0.0f);
		p.norm = glm::vec3(0.0f, 1.0f, 0.0f);
		p.color = emitterParams.startColor;
		p.size = emitterParams.startSize;
		p.activeTTL = 1;

		p.ttl = distTTL(gen);
		p.currentTtl = p.ttl * distPercent(gen);

		this->addParticle(p);
	}
}
