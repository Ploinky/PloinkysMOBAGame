#include "Map.h"
#include "Mesh.h"
#include "common/util.h"
#include "common/logger.h"
#include <fstream>
#include "Vertex.h"
#include <map>
#include "Direct3D.h"

namespace PMG {
    Map::~Map() {
        for (Mesh* mesh : m_meshes) {
            delete mesh;
        }
    }

    void Map::Load(std::string mapName) {
        std::string mapFileName = "./maps/";
        mapFileName.append(mapName).append("/").append(mapName).append(".omp");

		std::ifstream file(mapFileName);

		if (!file.is_open()) {
            Logger::Err("Could not open map file.");
            return;
        }

        TextureMesh* mesh = 0;
        int currIndex = 0;
        int currVertex = 0;
        std::map<std::string, std::string> textureFileNames;

        for (std::string line; std::getline(file, line); ) {
            std::list<std::string> tokens = Util::SplitString(line, ' ');
            
            std::string lineType = tokens.front();
            tokens.pop_front();

            if(!std::strcmp(lineType.c_str(), "omp")) {
                int ver = std::stoi(tokens.front());
                if (ver != MAP_VERSION) {
                    printf("Wrong version: %d\n", ver);
                    return;
                }
            }
            else if (!std::strcmp(lineType.c_str(), "o")) {
                currIndex = 0;
                currVertex = 0;
                mesh = new TextureMesh();
                m_meshes.push_back(mesh);
                mesh->indexCount = std::stoi(tokens.front());
                mesh->indices = (unsigned int*)malloc(sizeof(unsigned int) * mesh->indexCount);
                tokens.pop_front();
                mesh->vertexCount = std::stoi(tokens.front());
                mesh->vertices = (texture_shader_vertex_t*)malloc(sizeof(texture_shader_vertex_t) * mesh->vertexCount);
                tokens.pop_front();
                mesh->position.x = std::stof(tokens.front());
                tokens.pop_front();
                mesh->position.y = std::stof(tokens.front());
                tokens.pop_front();
                mesh->position.z = std::stof(tokens.front());
                tokens.pop_front();
                mesh->rotation.x = std::stof(tokens.front());
                tokens.pop_front();
                mesh->rotation.y = std::stof(tokens.front());
                tokens.pop_front();
                mesh->rotation.z = std::stof(tokens.front());
                tokens.pop_front();
            } else if (!std::strcmp(lineType.c_str(), "t")) {
                std::string textureName = tokens.front();
                tokens.pop_front();
                std::string textureFileName = "./maps/";
                textureFileName.append(mapName).append("/").append(textureName).append(".dds");
                mesh->m_textureFileName = textureFileName;
            } else if (!std::strcmp(lineType.c_str(), "f")) {
                mesh->indices[currIndex++] = std::stoi(tokens.front());
                tokens.pop_front();
                mesh->indices[currIndex++] = std::stoi(tokens.front());
                tokens.pop_front();
                mesh->indices[currIndex++] = std::stoi(tokens.front());
                tokens.pop_front();
            } else if (!std::strcmp(lineType.c_str(), "v")) {
                mesh->vertices[currVertex].position[0] = std::stof(tokens.front());
                tokens.pop_front();
                mesh->vertices[currVertex].position[1] = std::stof(tokens.front());
                tokens.pop_front();
                mesh->vertices[currVertex].position[2] = std::stof(tokens.front());
                tokens.pop_front();

                mesh->vertices[currVertex].texCoord[0] = std::stof(tokens.front());
                tokens.pop_front();
                mesh->vertices[currVertex].texCoord[1] = std::stof(tokens.front());
                tokens.pop_front();

                mesh->vertices[currVertex].normal[0] = 0;
                mesh->vertices[currVertex].normal[1] = 1;
                mesh->vertices[currVertex].normal[2] = 0;

                // FFS DO NOT FORGET THIS YOU FUCK
                currVertex++;
            }
        }
    }

    void Map::Initialize(Direct3D* direct3D) {
        for (Mesh* mesh : m_meshes) {
            mesh->Initialize(direct3D);
        }
    }

    std::list<Mesh*> Map::GetMeshes() {
        return m_meshes;
    }
}