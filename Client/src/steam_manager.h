#pragma once

#include <string>

namespace PMG {
	class SteamManager {
	public:
		bool Initialize();
		void RunCallbacks();
		std::string GetLaunchParameter(std::string param);
	};
}