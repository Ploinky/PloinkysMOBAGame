#include "settings.h"
#include "util.h"
#include "logger.h"

namespace PMG {
	int Settings::GetInt(const char* setting) {
		return std::stoi(GetInstance().m_settings[setting]);
	}

	bool Settings::GetBool(const char* setting) {
		return std::stoi(GetInstance().m_settings[setting]);
	}

	double Settings::GetDouble(const char* setting) {
		return std::stod(GetInstance().m_settings[setting]);
	}

	void Settings::SetInt(const char* setting, int value) {
		GetInstance().m_settings[setting] = std::to_string(value);
	}

	void Settings::SetBool(const char* setting, bool value) {
		GetInstance().m_settings[setting] = std::to_string(value);
	}

	void Settings::SetDouble(const char* setting, double value) {
		GetInstance().m_settings[setting] = std::to_string(value);
	}

	void Settings::LoadDefaults() {
		SetInt(PMGSettings::RESOLUTION_X, 1024);
		SetInt(PMGSettings::RESOLUTION_Y, 768);
		SetInt(PMGSettings::WINDOW_MODE, (int) WindowMode::BORDERLESS);
		SetInt(PMGSettings::MASTER_VOLUME, 1.0);
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
			} else if (!std::strcmp(tokens.front().c_str(), PMGSettings::RESOLUTION_X)) {
				SetInt(PMGSettings::RESOLUTION_X, std::stoi(tokens.back()));
			} else if (!std::strcmp(tokens.front().c_str(), PMGSettings::RESOLUTION_Y)) {
				SetInt(PMGSettings::RESOLUTION_Y, std::stoi(tokens.back()));
			} else if (!std::strcmp(tokens.front().c_str(), PMGSettings::MASTER_VOLUME)) {
				SetInt(PMGSettings::MASTER_VOLUME, std::stod(tokens.back()));
			}
		}
	}

	void Settings::SaveToFile(std::string fileName) {
		std::list<std::string> content;

		content.push_back(std::string(PMGSettings::WINDOW_MODE).append("=").append(std::to_string(GetInt(PMGSettings::WINDOW_MODE))));
		content.push_back(std::string(PMGSettings::RESOLUTION_X).append("=").append(std::to_string(GetInt(PMGSettings::RESOLUTION_X))));
		content.push_back(std::string(PMGSettings::RESOLUTION_Y).append("=").append(std::to_string(GetInt(PMGSettings::RESOLUTION_Y))));
		content.push_back(std::string(PMGSettings::MASTER_VOLUME).append("=").append(std::to_string(GetDouble(PMGSettings::MASTER_VOLUME))));
	
		Util::WriteLinesToFile(fileName, content);
	}
}