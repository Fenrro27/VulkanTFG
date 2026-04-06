#include "GECone.h"


//
// FUNCIÓN: GECone::GECone(int p, int m, float h, float r)
//
// PROPÓSITO: Crea la figura
//
GECone::GECone(int p, int m, float h, float r)
{
	int numFaces = 2 * p * m;     // Number of faces
	int numVertices = (p + 1) * m + 2;  // Number of vertices

	vertices.resize(numVertices);
	indices.resize(numFaces * 3);

	double module = sqrt(4 * h * h + r * r);
	double xyN = (float)(2 * h / module);
	double zN = (float)(r / module);

	int verticesIndex = 0;

	// Centro de la base
	vertices[0] = { { 0.0f, 0.0f, -h} , {0.0f, 0.0f,-1.0f}, {0.5f, 0.5f} };
	verticesIndex++;

	// Vértices de la base
	for (int i = 0; i < m; i++)
	{
		float x = (float)cos(glm::radians(360.0f * i / m));
		float y = -(float)sin(glm::radians(360.0f * i / m));
		vertices[verticesIndex] = { {x * r , y * r, -h}, {0.0f, 0.0f, -1.0f}, {x, y} };
		verticesIndex++;
	}

	// Extremo del cono
	vertices[verticesIndex] = { {0.0f, 0.0f, h}, {0.0f, 0.0f, 1.0f}, {0.5f, 0.5f} };
	verticesIndex++;

	// Vértices de los lados
	for (int i = 1; i <= p; i++)
	{
		float xy = i * r / p;
		float z = h - 2 * i * h / p;
		for (int j = 0; j < m; j++)
		{
			float xN = (float)cos(glm::radians(360.0f * j / m));
			float yN = (float)sin(glm::radians(360.0f * j / m));
			float x = (float)(xN * xy);
			float y = (float)(yN * xy);
			vertices[verticesIndex] = { { x, y, z}, {xN * xyN, yN * xyN, zN}, {x,y} };
			verticesIndex++;
		}
	}

	int indicesIndex = 0;
	// Base
	for (int i = 0; i < m - 1; i++)
	{
		indices[indicesIndex] = 0;
		indices[indicesIndex + 1] = i + 1;
		indices[indicesIndex + 2] = i + 2;
		indicesIndex += 3;
	}

	indices[indicesIndex] = 0;
	indices[indicesIndex + 1] = m;
	indices[indicesIndex + 2] = 1;
	indicesIndex += 3;

	// Extremo
	for (int i = 0; i < m - 1; i++)
	{
		indices[indicesIndex] = m + 1;
		indices[indicesIndex + 1] = m + 2 + i;
		indices[indicesIndex + 2] = m + 3 + i;
		indicesIndex += 3;
	}
	indices[indicesIndex] = m + 1;
	indices[indicesIndex + 1] = 2 * m + 1;
	indices[indicesIndex + 2] = m + 2;
	indicesIndex += 3;

	// Lados
	for (int j = 1; j < p; j++)
	{
		for (int i = 0; i < m - 1; i++)
		{
			indices[indicesIndex] = j * m + 2 + i;
			indices[indicesIndex + 1] = (j + 1) * m + 2 + i;
			indices[indicesIndex + 2] = (j + 1) * m + 3 + i;
			indicesIndex += 3;

			indices[indicesIndex] = j * m + 2 + i;
			indices[indicesIndex + 1] = (j + 1) * m + 3 + i;
			indices[indicesIndex + 2] = j * m + 3 + i;
			indicesIndex += 3;
		}

		indices[indicesIndex] = (j + 1) * m + 1;
		indices[indicesIndex + 1] = (j + 2) * m + 1;
		indices[indicesIndex + 2] = (j + 1) * m + 2;
		indicesIndex += 3;

		indices[indicesIndex] = (j + 1) * m + 1;
		indices[indicesIndex + 1] = (j + 1) * m + 2;
		indices[indicesIndex + 2] = j * m + 2;
		indicesIndex += 3;
	}
}