#pragma once
#include "GEFigure.h"
#include <string>

class GEModel : public GEFigure {
public:
    // Constructor que carga el archivo .obj
    GEModel(const std::string& path, float scale = 1.0f);
};