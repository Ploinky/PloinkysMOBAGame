#include "AssetManager.h"
#include <fstream>
#include <iostream>
namespace PMG {
	AssetManager::AssetManager() {
		LoadPakFiles();
	}

	AssetManager::~AssetManager() {
		for (PakFile* pakFile : files) {
			delete pakFile;
		}
	}

	void AssetManager::LoadPakFiles() {
		PakFile* clientPak = PakFile::Load("client.pak");
		files.push_back(clientPak);
	}

	std::vector<uint8_t> AssetManager::LoadFile(std::string fileName) {
		// check if any of our paks have the file
		for (PakFile* pakFile : files) {
			if (pakFile->HasFile(fileName)) {
				return pakFile->GetFileData(fileName);
			}
		}

		// file is not in any of the paks, so we check the file system
		
		// open file
		std::ifstream file(fileName, std::ios_base::binary | std::ios_base::in);

		// TODO error handling

		// skip to end of file, check how many bytes there are
		file.seekg(std::ios::end);
		int len = file.tellg();

		// create appropriate vector
		std::vector<uint8_t> data;
		data.resize(len);

		// back to start and read
		file.seekg(std::ios::beg);
		file.read((char*) data.data(), len);
	}
}