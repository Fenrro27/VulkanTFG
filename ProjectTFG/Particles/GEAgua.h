/**

 * @file GEAgua.h

 * @brief DefiniciÃ³n de la clase GEAgua para la simulaciÃ³n de partÃ­culas de agua.

 */

#ifndef GEAGUA_H

#define GEAGUA_H



#include "GEParticlesSystem.h"



/**

 * @class GEAgua

 * @brief Clase que representa un sistema de partÃ­culas de agua.

 * 

 * Esta clase hereda de GEParticlesSystem y configura los parÃ¡metros especÃ­ficos

 * para simular el comportamiento visual y fÃ­sico del agua (como una fuente).

 */

class GEAgua : public GEParticlesSystem

{

public:

    /**

     * @brief Constructor de la clase GEAgua.

     * @param particleCount NÃºmero inicial de partÃ­culas en el sistema.

     */

    explicit GEAgua(uint32_t particleCount = 200);

};



#endif