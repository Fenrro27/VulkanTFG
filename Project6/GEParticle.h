#pragma once

#include <glm\glm.hpp>


typedef  struct alignas(16)
{
    glm::vec4 color;      // 16 bytes
    glm::vec3 pos;        // 12 bytes
    float size;           // 4 bytes  (12 + 4 = 16, rellena el bloque anterior)
    glm::vec3 norm;       // 12 bytes
    int activeTTL;           // 4 bytes  (12 + 4 = 16, rellena el bloque anterior)
    float ttl;            // 4 bytes
    float currentTtl;     // 4 bytes
} GEParticle;
