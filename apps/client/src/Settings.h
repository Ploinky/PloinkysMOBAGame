#pragma once

#include <map>
#include <string>
#include <functional>
#include <core/platform/video_mode.h>
#include <vector>

class PMGSettings {
public:
	inline static std::string WINDOW_MODE = "window_mode";
	inline static std::string MASTER_VOLUME = "master_volume";
	inline static std::string VIDEO_MODE = "video_mode";
};

enum class WindowMode {
	WINDOWED,
	BORDERLESS,
	FULLSCREEN
};

class Settings {
public:
	int GetInt(std::string setting);
	bool GetBool(std::string setting);
	double GetDouble(std::string setting);
	std::string GetString(std::string setting);

	void SetInt(std::string setting, int value);
	void SetBool(std::string setting, bool value);
	void SetDouble(std::string setting, double value);
	void SetString(std::string setting, std::string value);

	void LoadDefaults();
	void LoadFromFile(std::string fileName);
	void SaveToFile(std::string fileName);
	Settings() {};

	std::vector<std::string> GetAllVideoModes();
	std::map<std::string, VideoMode_t>* GetAllVideoModesAndValues();

	std::function<void(std::string)> OnSettingChanged = std::bind([] {});

private:

	std::map<std::string, std::string> settings_;

	std::map<std::string, VideoMode_t> deviceModes_;
};

