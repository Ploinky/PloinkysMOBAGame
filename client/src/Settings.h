#pragma once

#include <map>
#include <string>
#include <functional>
#include <Windows.h>
#include <vector>

class PMGSettings {
public:
	static constexpr const char* WINDOW_MODE = "window_mode";
	static constexpr const char* MASTER_VOLUME = "master_volume";
	static constexpr const char* VIDEO_MODE = "video_mode";
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
	std::string GetString(const char* setting);

	void SetInt(const char* setting, int value);
	void SetBool(const char* setting, bool value);
	void SetDouble(const char* setting, double value);
	void SetString(const char* setting, std::string value);

	void LoadDefaults();
	void LoadFromFile(std::string fileName);
	void SaveToFile(std::string fileName);
	Settings() {};

	std::vector<std::string> GetAllVideoModes();
	std::map<std::string, DEVMODEA>* GetAllVideoModesAndValues();

	std::function<void(std::string)> OnSettingChanged = std::bind([] {});

private:

	std::map<const char*, std::string> settings_;

	std::map<std::string, DEVMODEA> deviceModes_;
};

