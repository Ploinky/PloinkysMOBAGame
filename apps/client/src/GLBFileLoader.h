#pragma once

#include <GLBFile.h>
#include <common/PMG_Common.h>
#include <common/PloinkysJSONLibrary.h>

class GLBFileLoader {
public:
	static GLBModel* LoadUsingLib(std::string modelName, AssetManager* assetManager);
};
