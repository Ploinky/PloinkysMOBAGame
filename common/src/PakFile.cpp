#include "PakFile.h"
#include "PakUtil.h"

PakFile* PakFile::Load(std::string pakFileName) {
	return PakUtil::UnpackFromPak(pakFileName);
}

bool PakFile::HasFile(std::string fileName) {
	for (PakFileTableEntry entry : entries) {
		if (std::strcmp(fileName.c_str(), entry.FilePath.c_str()) == 0) {
			return true;
		}
	}

	return false;
}

std::vector<uint8_t> PakFile::GetFileData(std::string fileName) {
	for (PakFileTableEntry entry : entries) {
		if (std::strcmp(fileName.c_str(), entry.FilePath.c_str()) == 0) {
			std::vector<uint8_t> outData;
			outData.resize(entry.Size);

			memcpy(outData.data(), data.data() + header.HeaderSize + entry.Offset, entry.Size);
			return outData;
		}
	}

	return {};
}