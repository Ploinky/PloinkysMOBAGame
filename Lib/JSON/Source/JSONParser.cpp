#include "../Include/JSONParser.h"
#include <cmath>
namespace PJL {
	JSONValue JSONParser::Parse(const std::string& jsonString) {
		const char* str = jsonString.c_str();
		return ParseValue(str);
	}

	JSONValue JSONParser::ParseValue(const char*& str) {
		SkipWhitespace(str);
		if (*str == '"') return ParseString(str);
		if (std::isdigit(*str) || *str == 'e' || *str == '-') return ParseNumber(str);
		if (*str == 't' || *str == 'f') return ParseBool(str);
		if (*str == 'n') return ParseNull(str);
		if (*str == '{') return ParseObject(str);
		if (*str == '[') return ParseArray(str);

		throw std::runtime_error("Invalid JSON value");
	}

	void JSONParser::SkipWhitespace(const char*& str) {
		while(std::isspace(*str)) ++str;
	}

	JSONValue JSONParser::ParseString(const char*& str) {
		std::string result;
		++str; // skip opening quote

		while(*str != '"') {
			result += *str++;
		}

		++str; // skip closing quote

		return JSONValue(result);
	}

	JSONValue JSONParser::ParseNumber(const char*& str) {
		std::string numStr;

		if (*str == 'e') {
			*str++;
			numStr += *str++;

			while (std::isdigit(*str) || *str == '-') {
				numStr += *str++;
			}

			int pow = std::stoi(numStr);
			
			return JSONValue(std::pow(10, pow));
		}

		if (*str == '-') {
			numStr += *str++;
		}

		while (std::isdigit(*str)) {
			numStr += *str++;
		}

		if (*str == '.') {
			numStr += *str++;

			while (std::isdigit(*str)) {
				numStr += *str++;
			}
			
			return JSONValue(std::stod(numStr));
		}

		return JSONValue(std::stoi(numStr));
	}


	JSONValue JSONParser::ParseBool(const char*& str) {
		if(std::strncmp(str, "true", 4) == 0) {
			str += 4;
			return JSONValue(true);
		}
		else if (std::strncmp(str, "false", 5) == 0) {
			str += 5;
			return JSONValue(false);
		}

		throw std::runtime_error("Invalid JSON boolean value");
	}

	JSONValue JSONParser::ParseNull(const char*& str) {
		if(std::strncmp(str, "null", 4) == 0) {
			str += 4;
			return JSONValue(nullptr);
		}

		throw std::runtime_error("Invalid JSON null value");
	}
	
	JSONValue JSONParser::ParseObject(const char*& str) {
		std::unique_ptr<JSONObject> object = std::make_unique<JSONObject>();
		++str; // skip opening brace

		SkipWhitespace(str);

		while(*str != '}') {
			if(*str == ',') {
				str++;
			}

			SkipWhitespace(str);

			JSONValue key = ParseString(str);

			SkipWhitespace(str);

			if(*str != ':') {
				throw std::runtime_error("Expected colon in JSON object");
			}

			++str;

			SkipWhitespace(str);

			JSONValue value = ParseValue(str);

			object->Set(key.AsString(), value);

			SkipWhitespace(str);
		}

		++str; // skip closing brace

		return JSONValue(*object);
	}

	JSONValue JSONParser::ParseArray(const char*& str) {
		std::unique_ptr<JSONArray> array = std::make_unique<JSONArray>();
		
		++str; // skip opening bracket

		SkipWhitespace(str);

		while(*str != ']') {
			if(*str == ',') {
				++str;
			}

			SkipWhitespace(str);

			array->Add(ParseValue(str));
		
			SkipWhitespace(str);
		}

		++str; // skip closing bracket

		return JSONValue(*array);
	}
}