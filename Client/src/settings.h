#pragma once

#include <map>
#include <string>

namespace PMG {
	class PMGSettings {
	public:
		static constexpr const char* RESOLUTION_X = "resolution_x";
		static constexpr const char* RESOLUTION_Y = "resolution_y";
		static constexpr const char* WINDOW_MODE = "window_mode";
		static constexpr const char* MASTER_VOLUME = "master_volume";
	};

	enum class WindowMode {
		WINDOWED,
		BORDERLESS,
		FULLSCREEN
	};

	class Settings {
	public:
		static int GetInt(const char* setting);
		static bool GetBool(const char* setting);
		static double GetDouble(const char* setting);

		static void SetInt(const char* setting, int value);
		static void SetBool(const char* setting, bool value);
		static void SetDouble(const char* setting, double value);

		static void LoadDefaults();
		static void LoadFromFile(std::string fileName);
		static void SaveToFile(std::string fileName);

	private:
		Settings() {};

		static Settings& GetInstance() {
			static Settings instance;

			return instance;
		}

		Settings(Settings const&);
		void operator=(Settings const&);

		std::map<const char*, std::string> m_settings;
	};
}

