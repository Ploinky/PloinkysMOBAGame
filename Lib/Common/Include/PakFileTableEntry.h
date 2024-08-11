#pragma once

#include <string>

namespace PMG {
	class PakFileTableEntry {
	public:
		int FilePathLength;
		std::string FilePath;
		int Offset = 0;
		int Size = 0;
	};
}