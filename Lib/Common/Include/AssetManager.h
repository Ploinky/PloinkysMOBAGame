#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include "PakFile.h"
#include <list>

namespace PMG {
	class AssetManager {
	public:
		AssetManager();
		~AssetManager();

		std::vector<PakFile*> files;

		void LoadPakFile(std::string fileName);

		std::vector<uint8_t> LoadFile(std::string fileName);
		std::list<std::string> LoadPlainFile(std::string fileName);
	private:
		void LoadPakFiles();
	};
}

#endif