#include <string>

namespace P3D {
    class Mesh;

    class Map {
    public:
        ~Map();

        void Load(std::string fileName);
        Mesh* GetMesh();
    private:
        Mesh* mesh;
        std::string mapName;
    };
}