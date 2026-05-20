/**

 * @file GEModel.h

 * @brief Archivo GEModel.h

 */

#pragma once
#include "GEObject.h"
#include <string>



/**

 * @class GEModel

 * @brief Class GEModel

 */

class GEModel : public GEObject {

public:

    // Constructor que carga el archivo .obj

    GEModel(GEGraphicsContext* gc, const std::string& path, float scale = 1.0f);

    /**

     * @brief FunciÃ³n initialize

     */

    void initialize(GEGraphicsContext* gc, GERenderingContext* rc); 

};