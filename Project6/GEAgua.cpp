#include "GEAgua.h"
#include <random>

GEAgua::GEAgua(uint32_t particleCount) : GEParticlesSystem()
{
    // 1. Usamos 'static' para que el generador se inicialice una sola vez 
        // y no en cada instancia de GEAgua.
    static std::random_device rd;
    static std::mt19937 gen(rd());

    // 2. Definimos distribuciones para los rangos específicos que necesitas
    // En lugar de (distr % 100) / 100.0f, usamos un rango [0.0, 1.0]
    std::uniform_real_distribution<float> distTTL(2.0f, 3.5f); // 2.0 + (0 a 1.5)
    std::uniform_real_distribution<float> distPercent(0.0f, 1.0f);


    // Supongamos que el tipo 2 es Agua en tu Compute Shader
    emitterParams.particleType = 2;

    // Posición inicial (base de la fuente)
    emitterParams.emitterPos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    // Fuerza inicial: Impulso hacia arriba (Y positivo)
    // Nota: El comportamiento de "caída" debería gestionarse en el shader 
    // aplicando una gravedad constante a la velocidad en cada frame.
    emitterParams.force = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);

    // Colores: Azul brillante a Azul transparente
    emitterParams.startColor = glm::vec4(0.2f, 0.5f, 1.0f, 0.8f);
    emitterParams.endColor = glm::vec4(0.7f, 0.9f, 1.0f, 0.2f);

    // Parámetros de dispersión y tamaño
    emitterParams.randomness = 0.1f;
    emitterParams.startSize = 1.0f;  // Gotas pequeñas
    emitterParams.endSize = 0.3f;

    emitterParams.time = 0.0f;
    emitterParams.deltaTime = 0.016f;

    // En el constructor de GEAgua::GEAgua
    for (uint32_t i = 0; i < particleCount; i++) {
        GEParticle p;
        p.pos = glm::vec3(0.0f, 0.0f, 0.0f);

        // Determinar a qué chorro pertenece (0, 90, 180, 270 grados)
        float angle90 = glm::radians(90.0f * (i % 4));
        float angle60 = glm::radians(60.0f);

        // Calcular vector de dirección: Inclinado 45º respecto al eje vertical (Y)
        // y rotado en el plano horizontal (XZ) cada 90º
        float vx = sin(angle60) * cos(angle90);
        float vy = cos(angle60);
        float vz = sin(angle60) * sin(angle90);

        p.norm = glm::vec3(vx, vy, vz) * 10.0f; // Multiplicar por la fuerza deseada
        p.color = emitterParams.startColor;
        p.size = emitterParams.startSize;
        p.activeTTL = 1;

        p.ttl = distTTL(gen);
        p.currentTtl = p.ttl * distPercent(gen);

        this->addParticle(p);
    }
}