#include <string>
#include <list>

#define MAP_VERSION 1

namespace PMG {
    class Mesh;

    class Map {
    public:
        ~Map();

        void Load(std::string mapName);
        std::list<Mesh*> GetMeshes();
    private:
        std::list<Mesh*> m_meshes;
        std::string mapName;
    };
}