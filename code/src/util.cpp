#include <fstream>
#include "util.hpp"
#include "logger.hpp"
#include <cstddef>

namespace P3D {
    file_t Util::ReadBytesFromFile(std::string fileName) {
		unsigned char* f = 0;

		// open the file
		std::ifstream file(fileName, std::ios::in | std::ios::binary | std::ios::ate);

		// if open was successful
		if (file.is_open())
		{
			// find the length of the file
			size_t length = file.tellg();
            
			// collect the file data
			f = new unsigned char[length];
			file.seekg(0, std::ios::beg);
			file.read(reinterpret_cast<char*>(f), length);
			file.close();
		    
            return {f, length};
		}

        Logger::Err("Could not load shader file!");
        return {0, 0};
    }

	std::list<std::string> Util::SplitString(std::string str, std::string delim) {
		std::list<std::string> tokens;

        size_t pos = 0;
        std::string token;
        while ((pos = str.find(delim)) != std::string::npos) {
            token = str.substr(0, pos);
            tokens.push_back(token);
            str.erase(0, pos + delim.length());
        }
        tokens.push_back(str);

		return tokens;
	}
}