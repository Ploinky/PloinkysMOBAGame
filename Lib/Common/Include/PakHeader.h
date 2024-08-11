#pragma once

#include <string>

namespace PMG {
	// Pak file header
	class PakHeader {
	public:
		// Magic string at beginning of file
		char Magic[4] = { 'P', 'A', 'K', 0 };
		// Initially version 0
		char PakVersion = 0;

		int HeaderSize;
		
		int EntryCount;
	};
}