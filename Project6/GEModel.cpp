#include "GEModel.h"
#include "tiny_obj_loader.h"
#include <stdexcept>
#include <iostream>
#include <unordered_map>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp> 

// Definición para que el mapa pueda comparar los vértices y encontrar duplicados
namespace std {
    template<> struct hash<GEVertex> {
        size_t operator()(GEVertex const& vertex) const {
            return ((hash<glm::vec3>()(vertex.pos) ^
                (hash<glm::vec3>()(vertex.norm) << 1)) >> 1) ^
                (hash<glm::vec2>()(vertex.tex) << 1);
        }
    };
}

GEModel::GEModel(const std::string& path, float scale) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;

    // Carga del archivo OBJ [cite: 608]
    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path.c_str(), ".", true)) {
        throw std::runtime_error("Error cargando OBJ: " + err); // [cite: 609]
    }

    // Mapa para rastrear vértices únicos y sus índices asociados
    std::unordered_map<GEVertex, uint32_t> uniqueVertices{};

    // Pre-reserva de memoria para evitar reasignaciones costosas durante la carga
    vertices.reserve(attrib.vertices.size() / 3);

    for (const auto& shape : shapes) { // 
        for (const auto& index : shape.mesh.indices) {
            GEVertex vertex{};

            // Carga de posición [cite: 610, 611]
            vertex.pos = {
                attrib.vertices[3 * index.vertex_index + 0] * scale,
                attrib.vertices[3 * index.vertex_index + 1] * scale,
                attrib.vertices[3 * index.vertex_index + 2] * scale
            };

            // Carga de normales si existen [cite: 611, 612]
            if (index.normal_index >= 0) {
                vertex.norm = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2]
                };
            }

            // Carga de coordenadas de textura (UV) con inversión para Vulkan [cite: 614]
            if (index.texcoord_index >= 0) {
                vertex.tex = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                };
            }

            // Optimización: Solo añadir el vértice si es nuevo. Si existe, solo añadir su índice.
            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(vertex);
            }

            indices.push_back(uniqueVertices[vertex]);
        }
    }
}