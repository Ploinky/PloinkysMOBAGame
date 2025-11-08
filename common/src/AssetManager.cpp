#include "AssetManager.h"
#include <fstream>
#include <iostream>
#include "util.h"
#include <exception>
#include <memory.h>
#include <cstdint>
#include "logger.h"

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
	// check if any of our paks have the file
	for (paklib::PakFile* pakFile : files) {
		if (pakFile->HasFile(fileName)) {
			return pakFile->GetFileData(fileName);
		}
	}
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
