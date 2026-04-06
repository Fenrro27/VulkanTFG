#pragma once
#include <glm/glm.hpp>

struct GEEmitterParams {
    alignas(16) glm::vec4 emitterPos;
    alignas(16) glm::vec4 force;
    alignas(16) glm::vec4 startColor;
    alignas(16) glm::vec4 endColor;
    alignas(4)  float deltaTime;
    alignas(4)  float randomness;
    alignas(4)  float startSize;
    alignas(4)  float endSize;
    alignas(4)  int   particleType;
    alignas(4)  float time;
};