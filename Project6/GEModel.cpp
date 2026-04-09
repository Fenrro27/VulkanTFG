#include "GEModel.h"
#include "tiny_obj_loader.h"
#include <stdexcept>
#include <iostream>

GEModel::GEModel(const std::string& path) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path.c_str(),".",true)) {
        throw std::runtime_error("Error cargando OBJ: " + err);
    }

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            GEVertex vertex{};

            // Posición 
            vertex.pos = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            };

            // Normales 
            if (index.normal_index >= 0) {
                vertex.norm = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2]
                };
            }

            // Coordenadas de textura 
            if (index.texcoord_index >= 0) {
                vertex.tex = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1] // Inversión para Vulkan
                };
            }

            vertices.push_back(vertex);
            // tinyobjloader carga por defecto listas de triángulos, usamos índices simples
            indices.push_back(indices.size());
        }
    }
}