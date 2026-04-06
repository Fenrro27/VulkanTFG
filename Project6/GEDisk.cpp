#include "GEDisk.h"


//
// FUNCIÓN: GEDisk::GEDisk(int p, int m, float r0, float r1)
//
// PROPÓSITO: Crea la figura
//
GEDisk::GEDisk(int p, int m, float r0, float r1)
{
    if (r0 == 0.0f)
    {
        int numFaces = 2 * (2 * m * p - m);     // Number of faces
        int numVertices = 2 * (m * p + 1);       // Number of vertices
        vertices.resize(numVertices);
        indices.resize(numFaces * 3);

        int verticesIndex = 0;
        int indicesIndex = 0;

        vertices[0] = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f}, {0.5f, 0.5f} };
        verticesIndex++;

        for (int j = 0; j < m; j++)
        {
            float r = (float)(r1 / p);

            float mCos = (float)cos(glm::radians(360.0f * j / m));
            float mSin = (float)sin(glm::radians(360.0f * j / m));
            vertices[verticesIndex] = { { mCos * r, mSin * r, 0.0f}, { 0.0f, 0.0f, 1.0f}, {0.5f + mCos, 0.5f + mSin} };
            verticesIndex++;

            indices[indicesIndex] = 0; // center
            indices[indicesIndex + 1] = j + 1;
            indices[indicesIndex + 2] = (j + 2 > m ? 1 : j + 2);
            indicesIndex += 3;
        }

        for (int i = 2; i <= p; i++)
        {
            for (int j = 0; j < m; j++)
            {
                float r = (float)(r1 * i / p);
                float mCos = (float)cos(glm::radians(360.0f * j / m));
                float mSin = (float)sin(glm::radians(360.0f * j / m));
                vertices[verticesIndex] = { { mCos * r, mSin * r, 0.0f}, { 0.0f, 0.0f, 1.0f}, {0.5f + mCos, 0.5f + mSin} };
                verticesIndex++;
            }
        }

        for (int i = 0; i < p - 1; i++)
        {
            for (int j = 0; j < m; j++)
            {
                indices[indicesIndex] = m * i + j + 1;
                indices[indicesIndex + 1] = m * (i + 1) + j + 1;
                indices[indicesIndex + 2] = (j + 1 == m ? m * (i + 1) + 1 : m * (i + 1) + j + 2);
                indicesIndex += 3;

                indices[indicesIndex] = m * i + j + 1;
                indices[indicesIndex + 1] = (j + 1 == m ? m * (i + 1) + 1 : m * (i + 1) + j + 2);
                indices[indicesIndex + 2] = (j + 1 == m ? m * i + 1 : m * i + j + 2);
                indicesIndex += 3;
            }
        }

        int base = verticesIndex;

        vertices[base + 0] = { { 0.0f, 0.0f, 0.0}, { 0.0f, 0.0f, -1.0f}, {0.5f, 0.5f} };
        verticesIndex++;

        for (int j = 0; j < m; j++)
        {
            float r = (float)(r1 / p);

            float mCos = (float)cos(glm::radians(360.0f * j / m));
            float mSin = (float)sin(glm::radians(360.0f * j / m));
            vertices[verticesIndex] = { { mCos * r, mSin * r, 0.0f }, { 0.0f, 0.0f, -1.0f}, {0.5f + mCos, 0.5f + mSin} };
            verticesIndex++;

            indices[indicesIndex] = base + 0; // center
            indices[indicesIndex + 1] = base + (j + 2 > m ? 1 : j + 2);
            indices[indicesIndex + 2] = base + j + 1;
            indicesIndex += 3;
        }

        for (int i = 2; i <= p; i++)
        {
            for (int j = 0; j < m; j++)
            {
                float r = (float)(r1 * i / p);
                float mCos = (float)cos(glm::radians(360.0f * j / m));
                float mSin = (float)sin(glm::radians(360.0f * j / m));
                vertices[verticesIndex] = { { mCos * r, mSin * r, 0.0f} , { 0.0f, 0.0f, -1.0f}, {0.5f + mCos, 0.5f + mSin} };
                verticesIndex++;
            }
        }

        for (int i = 0; i < p - 1; i++)
        {
            for (int j = 0; j < m; j++)
            {
                indices[indicesIndex] = base + m * i + j + 1;
                indices[indicesIndex + 1] = base + (j + 1 == m ? m * (i + 1) + 1 : m * (i + 1) + j + 2);
                indices[indicesIndex + 2] = base + m * (i + 1) + j + 1;
                indicesIndex += 3;

                indices[indicesIndex] = base + m * i + j + 1;
                indices[indicesIndex + 1] = base + (j + 1 == m ? m * i + 1 : m * i + j + 2);
                indices[indicesIndex + 2] = base + (j + 1 == m ? m * (i + 1) + 1 : m * (i + 1) + j + 2);
                indicesIndex += 3;
            }
        }
    }
    else
    {
        int numFaces = 2 * (2 * m * p);      // Number of faces
        int numVertices = 2 * (m * (p + 1));  // Number of vertices
        vertices.resize(numVertices);
        indices.resize(numFaces * 3);

        int verticesIndex = 0;
        int indicesIndex = 0;

        for (int i = 0; i <= p; i++)
        {
            float r = r0 + (r1 - r0) * i / p;
            for (int j = 0; j < m; j++)
            {
                float mCos = (float)cos(glm::radians(360.0f * j / m));
                float mSin = (float)sin(glm::radians(360.0f * j / m));
                vertices[verticesIndex] = { {mCos * r, mSin * r, 0.0f } , { 0.0f, 0.0f, 1.0f}, {0.5f + mCos, 0.5f + mSin} };
                verticesIndex++;
            }
        }

        for (int i = 0; i < p; i++)
        {
            for (int j = 0; j < m; j++)
            {
                indices[indicesIndex] = m * i + j;
                indices[indicesIndex + 1] = m * (i + 1) + j;
                indices[indicesIndex + 2] = (j + 1 == m ? m * (i + 1) : m * (i + 1) + j + 1);
                indicesIndex += 3;

                indices[indicesIndex] = m * i + j;
                indices[indicesIndex + 1] = (j + 1 == m ? m * (i + 1) : m * (i + 1) + j + 1);
                indices[indicesIndex + 2] = (j + 1 == m ? m * i : m * i + j + 1);
                indicesIndex += 3;
            }
        }

        int base = verticesIndex;

        for (int i = 0; i <= p; i++)
        {
            float r = r0 + (r1 - r0) * i / p;
            for (int j = 0; j < m; j++)
            {
                float mCos = (float)cos(glm::radians(360.0f * j / m));
                float mSin = (float)sin(glm::radians(360.0f * j / m));
                vertices[verticesIndex] = { { mCos * r, mSin * r, 0.0f} , { 0.0f, 0.0f, -1.0f}, {0.5f + mCos, 0.5f + mSin} };
                verticesIndex++;
            }
        }

        for (int i = 0; i < p; i++)
        {
            for (int j = 0; j < m; j++)
            {
                indices[indicesIndex] = base + m * i + j;
                indices[indicesIndex + 1] = base + (j + 1 == m ? m * (i + 1) : m * (i + 1) + j + 1);
                indices[indicesIndex + 2] = base + m * (i + 1) + j;
                indicesIndex += 3;

                indices[indicesIndex] = base + m * i + j;
                indices[indicesIndex + 1] = base + (j + 1 == m ? m * i : m * i + j + 1);
                indices[indicesIndex + 2] = base + (j + 1 == m ? m * (i + 1) : m * (i + 1) + j + 1);
                indicesIndex += 3;
            }
        }
    }
}
