#include "Settings.h"
#include <Common/PMG_Common.h>
#include <ranges>

namespace PMG {
	int Settings::GetInt(const char* setting) {
		return std::stoi(settings_[setting]);
	}

	bool Settings::GetBool(const char* setting) {
		return std::stoi(settings_[setting]);
	}

	double Settings::GetDouble(const char* setting) {
		return std::stod(settings_[setting]);
	}

	std::string Settings::GetString(const char* setting) {
		return settings_[setting];
	}

	void Settings::SetInt(const char* setting, int value) {
		settings_[setting] = std::to_string(value);
		OnSettingChanged(setting);
	}

	void Settings::SetBool(const char* setting, bool value) {
		settings_[setting] = std::to_string(value);
		OnSettingChanged(setting);
	}

	void Settings::SetDouble(const char* setting, double value) {
		settings_[setting] = std::to_string(value);
		OnSettingChanged(setting);
	}

	void Settings::SetString(const char* setting, std::string value) {
		settings_[setting] = value;
		OnSettingChanged(setting);
	}

	void Settings::LoadDefaults() {
		SetString(PMGSettings::VIDEO_MODE, "1024x768");
		SetInt(PMGSettings::WINDOW_MODE, (int) WindowMode::BORDERLESS);
		SetInt(PMGSettings::MASTER_VOLUME, 1.0);
		
		DEVMODEA devMode{};
		devMode.dmSize = sizeof(DEVMODE);
		for (int i = 0; EnumDisplaySettingsA(NULL, i, &devMode) != 0; i++) {
			printf("%d-%d\n", devMode.dmPelsWidth, devMode.dmPelsHeight);

			std::string newMode = std::to_string(devMode.dmPelsWidth).append("x").append(std::to_string(devMode.dmPelsHeight));

			bool exists = false;
			for (auto mode : deviceModes_) {
				if (std::strcmp(mode.first.c_str(), newMode.c_str()) == 0) {
					exists = true;
				}
			}

			if (!exists) {
				deviceModes_.emplace(newMode, devMode);
			}
		}

		printf("==============\n");
		for (auto mode : deviceModes_) {
			printf(std::string("mode: ").append(mode.first).append("\n").c_str());
		}
	}

	void Settings::LoadFromFile(std::string fileName) {
		std::list<std::string> fileContent = Util::ReadLinesFromFile(fileName);

		for (auto line : fileContent) {
			std::list<std::string> tokens = Util::SplitString(line, '=');

			if (tokens.size() != 2) {
				Logger::Err(std::string("Unexpected number of tokens in setting string ").append(line));
				continue;
			}

			if (!std::strcmp(tokens.front().c_str(), PMGSettings::WINDOW_MODE)) {
				SetInt(PMGSettings::WINDOW_MODE, std::stoi(tokens.back()));
			} else if (!std::strcmp(tokens.front().c_str(), PMGSettings::VIDEO_MODE)) {
				SetString(PMGSettings::VIDEO_MODE, tokens.back());
			} else if (!std::strcmp(tokens.front().c_str(), PMGSettings::MASTER_VOLUME)) {
				SetInt(PMGSettings::MASTER_VOLUME, std::stod(tokens.back()));
			}
		}
	}

	void Settings::SaveToFile(std::string fileName) {
		std::list<std::string> content;

		content.push_back(std::string(PMGSettings::WINDOW_MODE).append("=").append(std::to_string(GetInt(PMGSettings::WINDOW_MODE))));
		content.push_back(std::string(PMGSettings::VIDEO_MODE).append("=").append(GetString(PMGSettings::VIDEO_MODE)));
		content.push_back(std::string(PMGSettings::MASTER_VOLUME).append("=").append(std::to_string(GetDouble(PMGSettings::MASTER_VOLUME))));
	
		Util::WriteLinesToFile(fileName, content);
	}

	std::map<std::string, DEVMODEA>* Settings::GetAllVideoModesAndValues() {
		return &deviceModes_;
	}

	std::vector<std::string> Settings::GetAllVideoModes() {
		auto kv = std::views::keys(deviceModes_);
		std::vector<std::string> keys{ kv.begin(), kv.end() };

		return keys;
	}
}