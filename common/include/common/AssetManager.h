#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include <paklib/pak-file.h>
#include <list>

class AssetManager {
public:
	AssetManager();
	~AssetManager();

	std::vector<paklib::PakFile*> files;

	void LoadPakFile(std::string fileName);

	std::vector<uint8_t> LoadFile(std::string fileName);
	std::list<std::string> LoadPlainFile(std::string fileName);
private:
	void LoadPakFiles();
};

#endif