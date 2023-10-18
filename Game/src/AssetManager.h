#pragma once

#include "common/PakFile.h"

namespace PMG {
	class AssetManager {
	public:
		AssetManager();
		~AssetManager();

		std::vector<PakFile*> files;

		std::vector<uint8_t> LoadFile(std::string fileName);
	private:
		void LoadPakFiles();
	};
}