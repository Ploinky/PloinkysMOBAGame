#ifndef JSONPARSER_H
#define JSONPARSER_H

#include "JSONValue.h"
#include "JSONObject.h"
#include "JSONArray.h"
#include <stdexcept>

namespace PJL {
	class JSONParser {
	public:
		JSONValue Parse(const std::string& jsonString);
	private:
		JSONValue ParseValue(const char*& str);
		void SkipWhitespace(const char*& str);
		JSONValue ParseString(const char*& str);
		JSONValue ParseNumber(const char*& str);
		JSONValue ParseBool(const char*& str);
		JSONValue ParseNull(const char*& str);
		JSONValue ParseObject(const char*& str);
		JSONValue ParseArray(const char*& str);
	};
}

#endif