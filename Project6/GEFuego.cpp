#include "GEFuego.h"




GEFuego::GEFuego(uint32_t particleCount) : GEParticlesSystem()
{
	emitterParams.particleType = 1; // 1 = Fuego
	emitterParams.emitterPos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f); // Lo movemos para que no se pise con el humo, posicion en la escena
	emitterParams.force = glm::vec4(0.0f, 2.0f, 0.0f, 0.0f); // Sube 
	emitterParams.startColor = glm::vec4(1.0f, 0.8f, 0.1f, 1.0f); // Amarillo
	emitterParams.endColor = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);   // Rojo y transparente
	emitterParams.randomness = 2.0f; // Mucho caos lateral
	emitterParams.startSize = 3.0f;
	emitterParams.endSize = 0.5f; // Las llamas se encogen
	emitterParams.time = 0.0f;
	emitterParams.deltaTime = 0.016f;

	for (uint32_t i = 0; i < particleCount; i++) {
		GEParticle p;
		p.pos = glm::vec3(0.0f, 0.0f, 0.0f);
		p.norm = glm::vec3(0.0f, 1.0f, 0.0f);
		p.color = emitterParams.startColor;
		p.size = emitterParams.startSize;
		p.activeTTL = 1;

		p.ttl = ((rand() % 20) / 10.0f) + 0.5f;
		p.currentTtl = p.ttl * ((rand() % 100) / 100.0f);

		this->addParticle(p);
	}
}