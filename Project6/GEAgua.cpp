/**
 * @file GEAgua.cpp
 * @brief ImplementaciÃ³n de la clase GEAgua para la simulaciÃ³n de partÃ­culas de agua.
 */
#include "GEAgua.h"
#include <random>

/**
 * @brief Constructor de GEAgua.
 * 
 * Inicializa el sistema de partÃ­culas con parÃ¡metros de agua, configurando
 * la posiciÃ³n del emisor, colores, tamaÃ±os y la dispersiÃ³n inicial de las partÃ­culas.
 * 
 * @param particleCount Cantidad de partÃ­culas a generar.
 */
GEAgua::GEAgua(uint32_t particleCount) : GEParticlesSystem()
{
    // 1. Usamos 'static' para que el generador se inicialice una sola vez 
    // y no en cada instancia de GEAgua.
    static std::random_device rd;
    /**
     * @brief Generador de nÃºmeros aleatorios.
     */
    static std::mt19937 gen(rd());

    // 2. Definimos distribuciones para los rangos especÃ­ficos que necesitas
    /**
     * @brief DistribuciÃ³n para el tiempo de vida (TTL) de las partÃ­culas.
     */
    std::uniform_real_distribution<float> distTTL(2.0f, 3.5f); // 2.0 + (0 a 1.5)
    /**
     * @brief DistribuciÃ³n para el porcentaje inicial del TTL.
     */
    std::uniform_real_distribution<float> distPercent(0.0f, 1.0f);


    // Supongamos que el tipo 2 es Agua en el Compute Shader
    emitterParams.particleType = 2;

    // PosiciÃ³n inicial (base de la fuente)
    emitterParams.emitterPos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    // Fuerza inicial: Impulso hacia arriba (Y positivo)
    // Nota: El comportamiento de "caÃ­da" deberÃ­a gestionarse en el shader 
    // aplicando una gravedad constante a la velocidad en cada frame.
    emitterParams.force = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);

    // Colores: Azul brillante a Azul transparente
    emitterParams.startColor = glm::vec4(0.2f, 0.5f, 1.0f, 0.8f);
    emitterParams.endColor = glm::vec4(0.7f, 0.9f, 1.0f, 0.2f);

    // ParÃ¡metros de dispersiÃ³n y tamaÃ±o
    emitterParams.randomness = 0.1f;
    emitterParams.startSize = 1.0f;  // Gotas pequeÃ±as
    emitterParams.endSize = 0.3f;

    emitterParams.time = 0.0f;
    emitterParams.deltaTime = 0.016f;

    // InicializaciÃ³n de cada partÃ­cula individual
    for (uint32_t i = 0; i < particleCount; i++) {
        GEParticle p;
        p.pos = glm::vec3(0.0f, 0.0f, 0.0f);

        // Determinar a quÃ© chorro pertenece (0, 90, 180, 270 grados)
        float angle90 = glm::radians(90.0f * (i % 4));
        float angle60 = glm::radians(60.0f);

        // Calcular vector de direcciÃ³n: Inclinado 45Âº respecto al eje vertical (Y)
        // y rotado en el plano horizontal (XZ) cada 90Âº
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
