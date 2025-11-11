#include "JSONObject.h"

namespace PJL {
	
	JSONObject::~JSONObject() {

	}

	void JSONObject::Set(const std::string& key, const JSONValue& value) {
		object[key] = value;
	}

	const JSONValue& JSONObject::Get(const std::string& key) const {
		static JSONValue nullValue;
		
		auto it = object.find(key);
		
		if(it != object.end()) {
			return it->second;
		}

		return nullValue;
	}

	bool JSONObject::Contains(const std::string& key) const {
		return object.find(key) != object.end();
	}
}
