#pragma once

#include "PakFileTableEntry.h"
#include "PakHeader.h"
#include <vector>
#include <cstdint>
#include <string>

class PakFile {
public:
	PakHeader header;
	std::vector<PakFileTableEntry> entries;
	std::vector<uint8_t> data;

	static PakFile* Load(std::string pakFileName);

	bool HasFile(std::string fileName);
	std::vector<uint8_t> GetFileData(std::string fileName);
};