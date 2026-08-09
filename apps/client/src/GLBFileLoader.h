#pragma once

#include <common/PMG_Common.h>
#include <common/PloinkysJSONLibrary.h>

#include "GLBFile.h"

class GLBFileLoader {
public:
	static GLBModel* LoadUsingLib(std::string modelName, AssetManager* assetManager);
};
