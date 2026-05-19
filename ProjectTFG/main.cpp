/**

 * @file main.cpp

 * @brief Archivo main.cpp

 */

#include "GEApplication.h"

#include <iostream>

#include <stdexcept>



//

// PROYECTO: Project6

//

// DESCRIPCIÓN: Introduce el modelo de iluminación de Phong

//

int main()

{

	GEApplication app;



	try

	{

		app.run();

	}

	catch (const std::exception& e)

	{

		std::cerr << e.what() << std::endl;

		return EXIT_FAILURE;

	}



	return EXIT_SUCCESS;

}