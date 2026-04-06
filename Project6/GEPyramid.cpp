#include "GEPyramid.h"

//
// FUNCIÓN: GEPyramid::GEPyramid(float s)
//
// PROPÓSITO: Crea la figura
//
GEPyramid::GEPyramid(float s)
{
    glm::vec2 n = glm::normalize(glm::vec2(2.0f, 1.0f));

    vertices = {
      {{ 0.0f, 0.0f, s }, {n.x, 0.0f, n.y}, {0.5f, 1.0f}}, // E // Positive X
      {{ s, -s, -s }, {n.x, 0.0f, n.y}, {0.0f, 0.0f}},     // B  
      {{ s, s, -s }, {n.x, 0.0f, n.y}, {1.0f, 0.0f}},      // A

      {{ 0.0f, 0.0f, s }, {0.0f, n.x, n.y}, {0.5f, 1.0f}}, // E // Positive Y
      {{ s, s, -s }, {0.0f, n.x, n.y}, {0.0f, 0.0f}},      // A
      {{ -s, s, -s }, {0.0f, n.x, n.y}, {1.0f, 0.0f}},     // D

      {{ 0.0f, 0.0f, s }, { -n.x, 0.0f, n.y}, {0.5f, 1.0f}}, // E // Negative X
      {{ -s, s, -s }, { -n.x, 0.0f, n.y }, {0.0f, 0.0f}},     // D
      {{ -s, -s, -s }, { -n.x, 0.0f, n.y}, {1.0f, 0.0f}},    // C

      {{ 0.0f, 0.0f, s }, {0.0f, -n.x, n.y}, {0.5f, 1.0f}}, // E // Negative Y
      {{ -s, -s, -s }, {0.0f, -n.x, n.y}, {0.0f, 0.0f}},    // C
      {{ s, -s, -s }, {0.0f, -n.x, n.y}, {1.0f, 0.0f}},     // B  

      {{ s, s, -s }, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},      // A // Negative Z
      {{ s, -s, -s }, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},     // B  
      {{ -s, -s, -s }, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},    // C
      {{ -s, s, -s }, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}}     // D
    };

    indices = {  // Array of indexes
      0, 1, 2,
      3, 4, 5,
      6, 7, 8,
      9, 10, 11,
      12, 13, 14,
      12, 14, 15
    };
}
