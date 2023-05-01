#pragma once

namespace PMG {
	class SteamManager {
	public:
		bool Initialize();
		void Shutdown();
		void RunCallbacks();
	};
}