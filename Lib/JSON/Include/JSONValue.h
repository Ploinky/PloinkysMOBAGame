#ifndef JSONVALUE_H
#define JSONVALUE_H

#include <string>
#include <variant>
#include <memory>

namespace PJL {
	class JSONObject;
	class JSONArray;

	class JSONValue {
	public:
		using JSONData = std::variant<std::nullptr_t, bool, int, double, std::string, std::shared_ptr<JSONObject>, std::shared_ptr<JSONArray>>;

		JSONValue();
		JSONValue(bool value);
		JSONValue(int value);
		JSONValue(double value);
		JSONValue(const std::string& value);
		JSONValue(const char* value);
		JSONValue(const JSONObject& value);
		JSONValue(const JSONArray& value);

		bool IsNull() const;
		bool IsBool() const;
		bool IsInt() const;
		bool IsDouble() const;
		bool IsString() const;
		bool IsObject() const;
		bool IsArray() const;

		bool AsBool() const;
		int AsInt() const;
		double AsDouble() const;
		const std::string& AsString() const;
		const JSONObject& AsObject() const;
		const JSONArray& AsArray() const;
	private:
		JSONData data;
	};

}

#endif