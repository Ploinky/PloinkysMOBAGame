#pragma once

#include <map>
#include <string>
#include <functional>

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
		int GetInt(const char* setting);
		bool GetBool(const char* setting);
		double GetDouble(const char* setting);

		void SetInt(const char* setting, int value);
		void SetBool(const char* setting, bool value);
		void SetDouble(const char* setting, double value);

		void LoadDefaults();
		void LoadFromFile(std::string fileName);
		void SaveToFile(std::string fileName);
		Settings() {};

		std::function<void(std::string)> OnSettingChanged = std::bind([] {});

	private:

		std::map<const char*, std::string> settings_;
	};
}

