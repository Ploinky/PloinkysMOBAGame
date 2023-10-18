#include <stdio.h>

#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include "PakFile.h"
#include "PakUtil.h"

int main(char* argv[], int argc) {
	// PMG::PakFile* pak = PMG::PakFile::Load("client.pak");
	// 
	// if (pak->HasFile("models\\tower.p3d")) {
	// 	printf("Found tower file!");
	// } else{
	// 	printf("nope!");
	// }
	PMG::PakUtil::PackToPak("../../data/client/", "client.pak");
}