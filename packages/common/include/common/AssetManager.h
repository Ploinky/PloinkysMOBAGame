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
	
    // finds all files with the specified extension
    // searches plain files in debug and .pak files in release
    std::vector<std::string> GetFileNamesByExtension(const std::string strPathToSearch, const std::string strFileEnding);
private:
	void LoadPakFiles();
};

#endif