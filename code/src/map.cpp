#include "map.hpp"
#include "mesh.hpp"
#include "util.hpp"
#include "logger.hpp"
#include <fstream>
#include "vertex.hpp"

namespace P3D {
    Map::~Map() {
        delete mesh;
    }

    void Map::Load(std::string fileName) {
		std::ifstream file(fileName, std::ios::in | std::ios::binary);

		if (!file.is_open()) {
            Logger::Err("Could not open map file.");
            return;
        }

        char magic[3];

        file.read(magic, sizeof(char) * 3);

        if(magic[0] != 'o' || magic[1] != 'm' || magic[2] != 'p') {
            Logger::Err("Magic string at beginning of map file not found.");
            return;
        }

        int version;

        file.read((char*) &version, sizeof(int));

        if(version != 1) {
            Logger::Err("Map file has wrong version.");
            return;
        }

        mesh = new Mesh();

        file.read((char*) &mesh->vertexCount, sizeof(int));

        mesh->vertices = new Vertex[mesh->vertexCount];

        for(int i = 0; i < mesh->vertexCount; i++) {
            Vertex v;
            file.read((char*) &v.position[0], sizeof(float));
            file.read((char*) &v.position[1], sizeof(float));
            file.read((char*) &v.position[2], sizeof(float));

            v.color[0] = 0.5f;
            v.color[1] = 0.5f;
            v.color[2] = 0.5f;
            v.color[3] = 1.0f;
            
            mesh->vertices[i] = v;
        }

        file.read((char*) &mesh->indexCount, sizeof(int));

        mesh->indices = new unsigned int[mesh->indexCount];

        for(int i = 0; i < mesh->indexCount; i++) {
            file.read((char*) &mesh->indices[i], sizeof(unsigned int)); 
        }
    }

    Mesh* Map::GetMesh() {
        return mesh;
    }
}