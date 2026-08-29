#include <iostream>

#include <paklib/pak-util.h>

int main(int argc, char* argv[]) {
    std::cout << "pakr" << std::endl;

    if (argc != 3 && argc != 4) {
        std::cout << "invalid usage: " << argc << std::endl;
    }

    if (argc == 3) {
        paklib::PakUtil::PackToPak(argv[1], argv[2]);
    } else if (argc == 4) {
        paklib::PakUtil::PackToPak(argv[1], argv[2], argv[3]);
    }
}