#include "Settings.h"
#include <common/PMG_Common.h>
#include <ranges>
#include <core/platform/platform.h>

int Settings::GetInt(std::string setting) {
	return std::stoi(settings_[setting]);
}

bool Settings::GetBool(std::string setting) {
	return std::stoi(settings_[setting]);
}

double Settings::GetDouble(std::string setting) {
	return std::stod(settings_[setting]);
}

std::string Settings::GetString(std::string setting) {
	return settings_[setting];
}

void Settings::SetInt(std::string setting, int value) {
	settings_[setting] = std::to_string(value);
	OnSettingChanged(setting);
}

void Settings::SetBool(std::string setting, bool value) {
	settings_[setting] = std::to_string(value);
	OnSettingChanged(setting);
}

void Settings::SetDouble(std::string setting, double value) {
	settings_[setting] = std::to_string(value);
	OnSettingChanged(setting);
}

void Settings::SetString(std::string setting, std::string value) {
	settings_[setting] = value;
	OnSettingChanged(setting);
}

void Settings::LoadDefaults() {
	SetString(PMGSettings::VIDEO_MODE, "1024x768");
	SetInt(PMGSettings::WINDOW_MODE, (int) WindowMode::BORDERLESS);
	SetInt(PMGSettings::MASTER_VOLUME, 1.0);
	
	deviceModes_.clear();
	for(VideoMode_t& vidMode : CPlatform::GetAllVideoModes()) {
		std::string strName = std::to_string(vidMode.uWidth) + "x" + std::to_string(vidMode.uHeight) + "-" + std::to_string(vidMode.uColorDepth);
		deviceModes_.emplace(strName, vidMode);
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

		if (!tokens.front().compare(PMGSettings::WINDOW_MODE)) {
			SetInt(PMGSettings::WINDOW_MODE, std::stoi(tokens.back()));
		} else if (!tokens.front().compare(PMGSettings::VIDEO_MODE)) {
			SetString(PMGSettings::VIDEO_MODE, tokens.back());
		} else if (!tokens.front().compare(PMGSettings::MASTER_VOLUME)) {
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

std::map<std::string, VideoMode_t>* Settings::GetAllVideoModesAndValues() {
	return &deviceModes_;
}

std::vector<std::string> Settings::GetAllVideoModes() {
	auto kv = std::views::keys(deviceModes_);
	std::vector<std::string> keys{ kv.begin(), kv.end() };

	return keys;
}
