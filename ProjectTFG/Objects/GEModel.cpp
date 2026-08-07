/**

 * @file GEModel.cpp

 * @brief Archivo GEModel.cpp

 */

#include "GEModel.h"
#include "tiny_obj_loader.h"
#include <stdexcept>
#include <unordered_map>
#include <filesystem>
#include <sstream>
#include <algorithm>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include "commonDebug.h"
#include "GEAssets.h"


// Reader de materiales que obtiene el .mtl desde los recursos del motor
// (incrustados o de disco) y lo parsea desde memoria.

class GEMaterialReader : public tinyobj::MaterialReader
{

public:

	explicit GEMaterialReader(const std::string& baseDir) : m_baseDir(baseDir) {}

	bool operator()(const std::string& matId, std::vector<tinyobj::material_t>* materials,

		std::map<std::string, int>* matMap, std::string* warn, std::string* err) override
	{

		std::string key = m_baseDir + matId;

		std::replace(key.begin(), key.end(), '\\', '/');

		try
		{

			std::vector<char> data = GEAssets::getFileVector(key);

			std::istringstream mtlStream(std::string(data.begin(), data.end()));

			tinyobj::LoadMtl(matMap, materials, &mtlStream, warn, err);

			return true;
		}

		catch (...)
		{
			return false;
		}
	}

private:

	std::string m_baseDir;
};



// Especialización de hash para eliminar duplicados

namespace std {

    template<> struct hash<GEVertex> {

        /**

         * @brief Función operator

         */

        size_t operator()(GEVertex const& vertex) const {

            return ((hash<glm::vec3>()(vertex.pos) ^

                (hash<glm::vec3>()(vertex.norm) << 1)) >> 1) ^

                (hash<glm::vec2>()(vertex.tex) << 1);

        }

    };

}



// Actualizamos el constructor para recibir el contexto gráfico (necesario para las texturas)

GEModel::GEModel(GEGraphicsContext* gc, const std::string& path, float scale) {

    GE_DEBUG_INFO("Intentando cargar modelo: " << path);

    tinyobj::attrib_t attrib;

    std::vector<tinyobj::shape_t> shapes;

    std::vector<tinyobj::material_t> materials;

    std::string warn, err;

    // Obtenemos el directorio base para buscar el archivo .mtl y las texturas
    std::string directory = std::filesystem::path(path).parent_path().string() + "/";

    std::replace(directory.begin(), directory.end(), '\\', '/');

    std::string normPath = path;

    std::replace(normPath.begin(), normPath.end(), '\\', '/');

    // Cargamos el OBJ desde los recursos del motor (incrustados o de disco)
    std::vector<char> objData = GEAssets::getFileVector(normPath);

    std::istringstream objStream(std::string(objData.begin(), objData.end()));

    GEMaterialReader matReader(directory);

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, &objStream, &matReader, true)) {
        /**
         * @brief Función std::runtime_error
         */
        throw std::runtime_error("Error cargando OBJ: " + err);
    }



    // Cargamos todos los materiales y sus texturas asociadas

    std::vector<std::shared_ptr<GETexture>> modelTextures;

    for (const auto& mat : materials) {

        GE_DEBUG_INFO("Cargando Material OBJ: '" << mat.name << "' | Textura detectada: " << (mat.diffuse_texname.empty() ? "NINGUNA" : mat.diffuse_texname));

        if (!mat.diffuse_texname.empty()) {

            std::string texPath = directory + mat.diffuse_texname;

            std::replace(texPath.begin(), texPath.end(), '\\', '/');

            try {

                modelTextures.push_back(std::shared_ptr<GETexture>(

                    /**

                     * @brief Función GETexture

                     */

                    new GETexture(gc, texPath.c_str()),

                    [gc](GETexture* t) { t->destroy(gc); delete t; }

                ));

            }

            catch (...) {

                GE_DEBUG_ERROR("Fallo al cargar la textura: " << texPath << ". Usando wood.jpg"); // <-- Opcional, para ver si falla

                modelTextures.push_back(std::shared_ptr<GETexture>(

                    /**

                     * @brief Función GETexture

                     */

                    new GETexture(gc, "textures/wood.jpg"),

                    [gc](GETexture* t) { t->destroy(gc); delete t; }

                ));

                // Si falla la carga del archivo específico, usamos uno por defecto

            }

        }

        else {

            modelTextures.push_back(std::shared_ptr<GETexture>(

                /**

                 * @brief Función GETexture

                 */

                new GETexture(gc, "textures/wood.jpg"),

                [gc](GETexture* t) { t->destroy(gc); delete t; }

            ));

        }

    }



    // Procesamos cada "shape" del OBJ como una GEPiece independiente

    for (const auto& shape : shapes) {

        // Agrupamos vértices por material dentro de la misma shape
        int materialID = shape.mesh.material_ids.empty() ? -1 : shape.mesh.material_ids[0];



        auto piece = std::make_unique<GEPiece>();



        std::unordered_map<GEVertex, uint32_t> uniqueVertices{};



        std::vector<GEVertex> pieceVertices;

        std::vector<uint32_t> pieceIndices;



        for (const auto& index : shape.mesh.indices) {

            GEVertex vertex{};

            vertex.norm = { 0.0f, 1.0f, 0.0f };



            vertex.pos = {

                attrib.vertices[3 * index.vertex_index + 0] * scale,

                attrib.vertices[3 * index.vertex_index + 1] * scale,

                attrib.vertices[3 * index.vertex_index + 2] * scale

            };



            if (index.normal_index >= 0) {

                vertex.norm = {

                    attrib.normals[3 * index.normal_index + 0],

                    attrib.normals[3 * index.normal_index + 1],

                    attrib.normals[3 * index.normal_index + 2]

                };

            }



            if (index.texcoord_index >= 0) {

                vertex.tex = {

                    attrib.texcoords[2 * index.texcoord_index + 0],

                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1]

                };

            }



            if (uniqueVertices.count(vertex) == 0) {

                uniqueVertices[vertex] = static_cast<uint32_t>(pieceVertices.size());

                pieceVertices.push_back(vertex);

            }

            pieceIndices.push_back(uniqueVertices[vertex]);

        }



        // Asignamos los datos y la textura a la pieza

        piece->setVertices(pieceVertices); 

        piece->setIndices(pieceIndices);   



        if (materialID >= 0 && materialID < modelTextures.size()) {

            piece->setTexture(modelTextures[materialID]); 

        }



        this->pieces.push_back(std::move(piece)); // Añadimos la pieza al GEObject 

    }





}



/**

 * @brief Función GEModel::initialize

 */

void GEModel::initialize(GEGraphicsContext* gc, GERenderingContext* rc) {

    for (size_t i = 0; i < pieces.size(); i++) {

        if (pieces[i] != nullptr) {

            pieces[i]->initialize(gc, rc); 

        }

    }

}