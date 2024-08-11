#include <JSONValue.h>

namespace PJL {
		JSONValue::JSONValue() : data(nullptr) {};
		JSONValue::JSONValue(bool value) : data(value) {};
		JSONValue::JSONValue(int value) : data(value) {};
		JSONValue::JSONValue(double value) : data(value) {};
		JSONValue::JSONValue(const std::string& value) : data(value){};
		JSONValue::JSONValue(const char* value) : data(std::string(value)) {};

		bool JSONValue::IsNull() const { return std::holds_alternative<std::nullptr_t>(data); }
		bool JSONValue::IsBool() const { return std::holds_alternative<bool>(data); }
		bool JSONValue::IsInt() const { return std::holds_alternative<int>(data); }
		bool JSONValue::IsDouble() const { return std::holds_alternative<double>(data); }
		bool JSONValue::IsString() const { return std::holds_alternative<std::string>(data); }
		bool JSONValue::IsObject() const { return std::holds_alternative<std::shared_ptr<JSONObject>>(data); }
		bool JSONValue::IsArray() const { return std::holds_alternative<std::shared_ptr<JSONArray>>(data); }

		bool JSONValue::AsBool() const { return std::get<bool>(data); }
		int JSONValue::AsInt() const { return std::get<int>(data); }
		double JSONValue::AsDouble() const {
			if(IsInt()) {
				return AsInt();
			}
			return std::get<double>(data); 
		}
		const std::string& JSONValue::AsString() const { return std::get<std::string>(data); }
		const JSONObject& JSONValue::AsObject() const { return *std::get<std::shared_ptr<JSONObject>>(data); }
		const JSONArray& JSONValue::AsArray() const { return *std::get<std::shared_ptr<JSONArray>>(data); }

}
