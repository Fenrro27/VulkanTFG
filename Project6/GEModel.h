#pragma once
#include "GEObject.h"
#include <string>

class GEModel : public GEObject {
public:
    // Constructor que carga el archivo .obj
    GEModel(GEGraphicsContext* gc, const std::string& path, float scale = 1.0f);
    void initialize(GEGraphicsContext* gc, GERenderingContext* rc); 
};