#include "AssetManager.h"
#include <fstream>
#include <iostream>
#include "util.h"
#include <exception>
#include <memory.h>
#include <cstdint>

AssetManager::AssetManager() {
}

AssetManager::~AssetManager() {
	for (PakFile* pakFile : files) {
		delete pakFile;
	}
}

void AssetManager::LoadPakFile(std::string fileName) {
	files.push_back(PakFile::Load(fileName));
}

std::vector<uint8_t> AssetManager::LoadFile(std::string fileName) {
#ifdef _DEBUG
	file_t f = Util::ReadBytesFromFile(fileName);
	std::vector<uint8_t> ret;
	ret.resize(f.size);
	memcpy(ret.data(), f.data, ret.size());
	return ret;
#endif
	// check if any of our paks have the file
	for (PakFile* pakFile : files) {
		if (pakFile->HasFile(fileName)) {
			return pakFile->GetFileData(fileName);
		}
	}

	// throw std::runtime_error(std::string("Failed to load asset <").append(fileName).append(">").c_str());
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