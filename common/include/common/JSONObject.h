#ifndef JSONOBJECT_H
#define JSONOBJECT_H

#include "JSONValue.h"
#include <map>
#include <string>

namespace PJL {
	class JSONObject {
	public:
		~JSONObject();
		void Set(const std::string& key, const JSONValue& value);

		const JSONValue& Get(const std::string& key) const;

		bool Contains(const std::string& key) const;

	private:
		std::map<std::string, JSONValue> object;

	};
	inline JSONValue::JSONValue(const JSONObject& value) : data(std::make_shared<JSONObject>(value)) {};
}

#endif