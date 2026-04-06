#ifndef GEAGUA_H
#define GEAGUA_H

#include "GEParticlesSystem.h"

class GEAgua : public GEParticlesSystem
{
public:
    explicit GEAgua(uint32_t particleCount = 100);
};

#endif