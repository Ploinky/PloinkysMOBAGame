#pragma once

#include <string>
#include <vector>
#include <cstddef>
#include <list>

namespace P3D {
    struct file_t {
        unsigned char* data;
        size_t size;
    };

    class Util {
        public:
            static file_t ReadBytesFromFile(std::string file);
	        static std::list<std::string> SplitString(std::string str, std::string delim);
    };
}