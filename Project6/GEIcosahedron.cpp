#include "GEIcosahedron.h"

#include "GEVertex.h"

//
// FUNCIÓN: GEIcosahedron::GEIcosahedron(float r)
//
// PROPÓSITO: Crea la figura
//
GEIcosahedron::GEIcosahedron(float r)
{
	int numFaces = 20;         // Number of faces
	int numVertices = 60;  // Number of vertices
	vertices.resize(numVertices);
	indices.resize(numFaces * 3);

	float phi = (float)((1 + sqrt(5.0)) / 2.0);
	float mod = (float)sqrt(6 + 9 * phi);
	float p0M = (float)phi / mod;
	float p1M = (float)(phi + 1) / mod;
	float p2M = (float)(2 * phi + 1) / mod;

	float X0 = 0.0f;
	float X1 = 1.0f / 11;
	float X2 = 2.0f / 11;
	float X3 = 3.0f / 11;
	float X4 = 4.0f / 11;
	float X5 = 5.0f / 11;
	float X6 = 6.0f / 11;
	float X7 = 7.0f / 11;
	float X8 = 8.0f / 11;
	float X9 = 9.0f / 11;
	float X10 = 10.0f / 11;
	float X11 = 1.0f;
	float Y0 = 0.0f;
	float Y1 = 1.0f / 3;
	float Y2 = 2.0f / 3;
	float Y3 = 1.0f;

	glm::vec2 T0 = { X1, Y3 };
	glm::vec2 T1 = { X0, Y2 };
	glm::vec2 T2 = { X2, Y2 };
	glm::vec2 T3 = { X3, Y3 };
	glm::vec2 T4 = { X2, Y2 };
	glm::vec2 T5 = { X4, Y2 };
	glm::vec2 T6 = { X5, Y3 };
	glm::vec2 T7 = { X4, Y2 };
	glm::vec2 T8 = { X6, Y2 };
	glm::vec2 T9 = { X7, Y3 };
	glm::vec2 T10 = { X6, Y2 };
	glm::vec2 T11 = { X8, Y2 };
	glm::vec2 T12 = { X9, Y3 };
	glm::vec2 T13 = { X8, Y2 };
	glm::vec2 T14 = { X10, Y2 };
	glm::vec2 T15 = { X7, Y1 };
	glm::vec2 T16 = { X9, Y1 };
	glm::vec2 T17 = { X8, Y2 };
	glm::vec2 T18 = { X7, Y1 };
	glm::vec2 T19 = { X8, Y2 };
	glm::vec2 T20 = { X6, Y2 };
	glm::vec2 T21 = { X7, Y1 };
	glm::vec2 T22 = { X6, Y2 };
	glm::vec2 T23 = { X5, Y1 };
	glm::vec2 T24 = { X7, Y1 };
	glm::vec2 T25 = { X5, Y1 };
	glm::vec2 T26 = { X6, Y0 };
	glm::vec2 T27 = { X7, Y1 };
	glm::vec2 T28 = { X8, Y0 };
	glm::vec2 T29 = { X9, Y1 };
	glm::vec2 T30 = { X8, Y2 };
	glm::vec2 T31 = { X9, Y1 };
	glm::vec2 T32 = { X10, Y2 };
	glm::vec2 T33 = { X6, Y2 };
	glm::vec2 T34 = { X4, Y2 };
	glm::vec2 T35 = { X5, Y1 };
	glm::vec2 T36 = { X10, Y2 };
	glm::vec2 T37 = { X9, Y1 };
	glm::vec2 T38 = { X11, Y1 };
	glm::vec2 T39 = { X0, Y2 };
	glm::vec2 T40 = { X1, Y1 };
	glm::vec2 T41 = { X2, Y2 };
	glm::vec2 T42 = { X4, Y2 };
	glm::vec2 T43 = { X2, Y2 };
	glm::vec2 T44 = { X3, Y1 };
	glm::vec2 T45 = { X4, Y2 };
	glm::vec2 T46 = { X3, Y1 };
	glm::vec2 T47 = { X5, Y1 };
	glm::vec2 T48 = { X2, Y2 };
	glm::vec2 T49 = { X1, Y1 };
	glm::vec2 T50 = { X3, Y1 };
	glm::vec2 T51 = { X11, Y1 };
	glm::vec2 T52 = { X9, Y1 };
	glm::vec2 T53 = { X10, Y0 };
	glm::vec2 T54 = { X1, Y1 };
	glm::vec2 T55 = { X2, Y0 };
	glm::vec2 T56 = { X3, Y1 };
	glm::vec2 T57 = { X4, Y0 };
	glm::vec2 T58 = { X5, Y1 };
	glm::vec2 T59 = { X3, Y1 };

	glm::vec3 N1 = { -p0M, p2M, 0.0f };
	glm::vec3 N2 = { -p1M, p1M, p1M };
	glm::vec3 N3 = { 0.0f, p0M, p2M };
	glm::vec3 N4 = { p1M, p1M, p1M };
	glm::vec3 N5 = { p0M, p2M, 0.0f };
	glm::vec3 N6 = { p2M, 0.0f, -p0M };
	glm::vec3 N7 = { p2M, 0.0f, p0M };
	glm::vec3 N8 = { p1M, -p1M, p1M };
	glm::vec3 N9 = { p0M, -p2M, 0.0f };
	glm::vec3 N10 = { p1M, -p1M, -p1M };
	glm::vec3 N11 = { p1M, p1M, -p1M };
	glm::vec3 N12 = { 0.0f, -p0M, p2M };
	glm::vec3 N13 = { 0.0f, p0M, -p2M };
	glm::vec3 N14 = { -p1M, p1M, -p1M };
	glm::vec3 N15 = { -p2M, 0.0f, p0M };
	glm::vec3 N16 = { -p1M, -p1M, p1M };
	glm::vec3 N17 = { -p2M, 0.0f, -p0M };
	glm::vec3 N18 = { 0.0f, -p0M, -p2M };
	glm::vec3 N19 = { -p1M, -p1M, -p1M };
	glm::vec3 N20 = { -p0M, -p2M, 0.0f };

	glm::vec3 A0 = { 0.0f, r * phi, r };
	glm::vec3 A1 = { 0.0f, r * phi, -r };
	glm::vec3 A2 = { 0.0f, -r * phi, -r };
	glm::vec3 A3 = { 0.0f, -r * phi, r };
	glm::vec3 B0 = { r * phi, r, 0.0f };
	glm::vec3 B1 = { r * phi, -r, 0.0f };
	glm::vec3 B2 = { -r * phi, -r, 0.0f };
	glm::vec3 B3 = { -r * phi, r, 0.0f };
	glm::vec3 C0 = { r, 0.0f, r * phi };
	glm::vec3 C1 = { r, 0.0f, -r * phi };
	glm::vec3 C2 = { -r, 0.0f, -r * phi };
	glm::vec3 C3 = { -r, 0.0f, r * phi };

	// face 1
	vertices[0] = { A0, N1, T0 };
	vertices[1] = { A1, N1, T1 };
	vertices[2] = { B3, N1, T2 };

	// face 2
	vertices[3] = { A0, N2, T3 };
	vertices[4] = { B3, N2, T4 };
	vertices[5] = { C3, N2, T5 };

	// face 3
	vertices[6] = { A0, N3, T6 };
	vertices[7] = { C3, N3, T7 };
	vertices[8] = { C0, N3, T8 };

	// face 4
	vertices[9] = { A0, N4, T9 };
	vertices[10] = { C0, N4, T10 };
	vertices[11] = { B0, N4, T11 };

	// face 5
	vertices[12] = { A0, N5, T12 };
	vertices[13] = { B0, N5, T13 };
	vertices[14] = { A1, N5, T14 };

	// face 6
	vertices[15] = { B1, N6, T15 };
	vertices[16] = { C1, N6, T16 };
	vertices[17] = { B0, N6, T17 };

	// face 7
	vertices[18] = { B1, N7, T18 };
	vertices[19] = { B0, N7, T19 };
	vertices[20] = { C0, N7, T20 };

	// face 8
	vertices[21] = { B1, N8, T21 };
	vertices[22] = { C0, N8, T22 };
	vertices[23] = { A3, N8, T23 };

	// face 9
	vertices[24] = { B1, N9, T24 };
	vertices[25] = { A3, N9, T25 };
	vertices[26] = { A2, N9, T26 };

	// face 10
	vertices[27] = { B1, N10, T27 };
	vertices[28] = { A2, N10, T28 };
	vertices[29] = { C1, N10, T29 };

	// face 11
	vertices[30] = { B0, N11, T30 };
	vertices[31] = { C1, N11, T31 };
	vertices[32] = { A1, N11, T32 };

	// face 12
	vertices[33] = { C0, N12, T33 };
	vertices[34] = { C3, N12, T34 };
	vertices[35] = { A3, N12, T35 };

	// face 13
	vertices[36] = { A1, N13, T36 };
	vertices[37] = { C1, N13, T37 };
	vertices[38] = { C2, N13, T38 };

	// face 14
	vertices[39] = { A1, N14, T39 };
	vertices[40] = { C2, N14, T40 };
	vertices[41] = { B3, N14, T41 };

	// face 15
	vertices[42] = { C3, N15, T42 };
	vertices[43] = { B3, N15, T43 };
	vertices[44] = { B2, N15, T44 };

	// face 16
	vertices[45] = { C3, N16, T45 };
	vertices[46] = { B2, N16, T46 };
	vertices[47] = { A3, N16, T47 };

	// face 17
	vertices[48] = { B3, N17, T48 };
	vertices[49] = { C2, N17, T49 };
	vertices[50] = { B2, N17, T50 };

	// face 18
	vertices[51] = { C2, N18, T51 };
	vertices[52] = { C1, N18, T52 };
	vertices[53] = { A2, N18, T53 };

	// face 19
	vertices[54] = { C2, N19, T54 };
	vertices[55] = { A2, N19, T55 };
	vertices[56] = { B2, N19, T56 };

	// face 20
	vertices[57] = { A2, N20, T57 };
	vertices[58] = { A3, N20, T58 };
	vertices[59] = { B2, N20, T59 };

	for (int i = 0; i < numVertices; i++) indices[i] = i;
}
