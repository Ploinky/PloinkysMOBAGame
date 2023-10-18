#pragma once

#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS

#include <string>
#include <vector>
#include <cstddef>
#include <list>
#include <codecvt>

namespace PMG {
    struct file_t {
        unsigned char* data;
        size_t size;
    };

    class Util {
    public:
        static file_t ReadBytesFromFile(std::string file);
        static std::list<std::string> ReadLinesFromFile(std::string fileName);
        static void WriteLinesToFile(std::string fileName, std::list<std::string> content);
	    static std::list<std::string> SplitString(std::string str, std::string delim);
        static std::list<std::string> SplitString(std::string str, char delim);
        static long long GetSystemTime();
    };
}