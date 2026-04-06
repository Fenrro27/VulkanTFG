#include "GEHumo.h"
#include <random>




GEHumo::GEHumo(uint32_t particleCount) : GEParticlesSystem()
{
	// 1. Usamos 'static' para que el generador se inicialice una sola vez 
	  // y no en cada instancia de GEAgua.
	static std::random_device rd;
	static std::mt19937 gen(rd());

	// 2. Definimos distribuciones para los rangos específicos que necesitas
	// En lugar de (distr % 100) / 100.0f, usamos un rango [0.0, 1.0]
	std::uniform_real_distribution<float> distTTL(2.0f, 6.0f); 
	std::uniform_real_distribution<float> distPercent(0.0f, 1.0f);



	// Configurar valores de Humo para la emision de particulas
	emitterParams.particleType = 0; // 0 = Humo
	emitterParams.emitterPos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	emitterParams.force = glm::vec4(0.0f, 2.0f, 0.0f, 0.0f); // Fuerza hacia arriba
	emitterParams.startColor = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f); // Gris
	emitterParams.endColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);   // Negro y transparente
	emitterParams.randomness = 1.5f;
	emitterParams.startSize = 1.5f;
	emitterParams.endSize = 15.0f;
	emitterParams.time = 0.0f;
	emitterParams.deltaTime = 0.016f; // Valor de seguridad inicial

	for (uint32_t i = 0; i < particleCount; i++) {
		GEParticle p;
		p.pos = glm::vec3(0.0f, 0.0f, 0.0f);
		p.norm = glm::vec3(0.0f, 1.0f, 0.0f);
		p.color = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f); 
		p.size = 2.0f;
		p.activeTTL = 1;

		p.ttl = distTTL(gen);
		p.currentTtl = p.ttl * distPercent(gen);

		this->addParticle(p);
	}
}