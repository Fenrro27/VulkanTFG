#pragma once

//
// ESTRUCTURA: GETransform
//
// DESCRIPCIÓN: Estructura que almacena los datos necesarios para girar las figuras
//
struct GETransform
{
	alignas(16) glm::mat4 MVP;
	alignas(16) glm::mat4 ViewMatrix;
	alignas(16) glm::mat4 ModelViewMatrix;
};