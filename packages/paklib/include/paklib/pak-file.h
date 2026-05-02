#pragma once

#include "pak-file-table-entry.h"
#include "pak-header.h"
#include <vector>
#include <cstdint>
#include <string>

namespace paklib {
	class PakFile {
	public:
		PakHeader header;
		std::vector<PakFileTableEntry> entries;
		std::vector<uint8_t> data;
	
		static PakFile* Load(std::string pakFileName);
	
		bool HasFile(std::string fileName);
		std::vector<uint8_t> GetFileData(std::string fileName);
	};
}