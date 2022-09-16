#include "map.hpp"
#include "mesh.hpp"
#include "util.hpp"
#include "logger.hpp"
#include <fstream>
#include "vertex.hpp"
#include <map>
namespace P3D {
    Map::~Map() {
    }

    void Map::Load(std::string fileName) {
		std::ifstream file(fileName, std::ios::in);

		if (!file.is_open()) {
            Logger::Err("Could not open map file.");
            return;
        }

        TexturedStaticMesh* mesh;
        int currIndex = 0;
        int currVertex = 0;
        std::map<std::string, std::string> textureFileNames;

        for (std::string line; std::getline(file, line); ) {
            std::list<std::string> tokens = Util::SplitString(line, ' ');
            printf("%s\n", line.c_str());
            
            std::string lineType = tokens.front();
            tokens.pop_front();

            if(!std::strcmp(lineType.c_str(), "omp")) {
                int ver = std::stoi(tokens.front());
                if (ver != MAP_VERSION) {
                    printf("Wrong version: %d\n", ver);
                    return;
                }
            }
            else if (!std::strcmp(lineType.c_str(), "obj")) {
                currIndex = 0;
                currVertex = 0;
                mesh = new TexturedStaticMesh();
                m_meshes.push_back(mesh);
                mesh->indexCount = std::stoi(tokens.front()) * 3;
                mesh->indices = (unsigned int*)malloc(sizeof(unsigned int) * mesh->indexCount);
                tokens.pop_front();
                mesh->vertexCount = std::stoi(tokens.front());
                mesh->vertices = (Vertex*)malloc(sizeof(Vertex) * mesh->vertexCount);
                tokens.pop_front();
            } else if (!std::strcmp(lineType.c_str(), "m")) {
                std::string textureName = tokens.front();
                tokens.pop_front();
                std::string textureFileName = tokens.front();
                textureFileName = std::string(".").append(textureFileName);
                tokens.pop_front();
                textureFileNames.insert({ textureName, textureFileName });
            } else if (!std::strcmp(lineType.c_str(), "t")) {
                std::string textureName = tokens.front();
                tokens.pop_front();
                std::string textureFileName = textureFileNames.at(textureName);
                mesh->textureFileName = textureFileName;
            } else if (!std::strcmp(lineType.c_str(), "f")) {
                mesh->indices[currIndex++] = std::stoi(tokens.front());
                tokens.pop_front();
                mesh->indices[currIndex++] = std::stoi(tokens.front());
                tokens.pop_front();
                mesh->indices[currIndex++] = std::stoi(tokens.front());
                tokens.pop_front();
            } else if (!std::strcmp(lineType.c_str(), "v")) {
                mesh->vertices[currVertex].color[0] = 0;
                mesh->vertices[currVertex].color[1] = 255;
                mesh->vertices[currVertex].color[2] = 0;
                mesh->vertices[currVertex].color[3] = 255;

                mesh->vertices[currVertex].position[0] = std::stof(tokens.front());
                tokens.pop_front();
                mesh->vertices[currVertex].position[1] = std::stof(tokens.front());
                tokens.pop_front();
                mesh->vertices[currVertex++].position[2] = std::stof(tokens.front());
                tokens.pop_front();
            }
        }
    }

    std::list<Mesh*> Map::GetMeshes() {
        return m_meshes;
    }
}