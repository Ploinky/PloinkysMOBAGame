#include "AssetManager.h"
#include <fstream>
#include <iostream>
#include "util.h"
#include <exception>
#include <memory.h>
#include <cstdint>
#include "logger.h"
#include <filesystem>

AssetManager::AssetManager() {
}

AssetManager::~AssetManager() {
	for (paklib::PakFile* pakFile : files) {
		delete pakFile;
	}
}

void AssetManager::LoadPakFile(std::string fileName) {
	files.push_back(paklib::PakFile::Load(fileName));
}

std::vector<uint8_t> AssetManager::LoadFile(std::string fileName) {
#ifdef _DEBUG
	std::ifstream inputFile(fileName, std::ios_base::binary);

    inputFile.seekg(0, std::ios_base::end);
    auto length = inputFile.tellg();
    inputFile.seekg(0, std::ios_base::beg);

    // Make a buffer of the exact size of the file and read the data into it.
    std::vector<uint8_t> buffer(length);
    inputFile.read(reinterpret_cast<char*>(buffer.data()), length);

    inputFile.close();
    return buffer;
#else
	// check if any of our paks have the file
	for (paklib::PakFile* pakFile : files) {
		if (pakFile->HasFile(fileName)) {
			return pakFile->GetFileData(fileName);
		}
	}
#endif
	Logger::FormatErr("Failed to load asset <%s>", fileName.c_str());
	return {};
}

std::list<std::string> AssetManager::LoadPlainFile(std::string fileName) {
	std::vector<uint8_t> bytes = LoadFile(fileName);

	std::list<std::string> content;
	std::string currLine = "";

	for (int i = 0; i < bytes.size(); i++) {
		char c = bytes[i];

		if (c == '\n' || c == '\r') {
			if (currLine.length() > 0) {
				content.push_back(currLine);
			}
			currLine = "";
		}
		else {
			currLine += c;
		}
	}
	
	if (currLine.length() > 0) {
		content.push_back(currLine);
	}

	return content;
}


std::vector<std::string> AssetManager::GetFileNamesByExtension(const std::string strPathToSearch, const std::string strFileEnding) {
    std::vector<std::string> vecFileNames;

    for(std::filesystem::directory_entry entry : std::filesystem::directory_iterator(strPathToSearch)) {
        if(entry.is_directory()) {
            std::vector<std::string> vecFoundInDir = GetFileNamesByExtension(entry.path().string(), strFileEnding);
            if(!vecFoundInDir.empty()) {
                vecFileNames.insert(vecFileNames.end(), vecFoundInDir.begin(), vecFoundInDir.end());
            }
        }

        if(entry.is_regular_file() && !entry.path().extension().string().compare(strFileEnding)) {
            vecFileNames.push_back(strPathToSearch + "/" + entry.path().filename().string());
        }
    }

    return vecFileNames;
}