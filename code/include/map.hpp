#include <string>
#include <list>

#define MAP_VERSION 1

namespace P3D {
    class Mesh;

    class Map {
    public:
        ~Map();

        void Load(std::string fileName);
        std::list<Mesh*> GetMeshes();
    private:
        std::list<Mesh*> m_meshes;
        std::string mapName;
    };
}