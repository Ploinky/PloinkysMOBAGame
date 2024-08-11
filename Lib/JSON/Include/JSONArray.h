#ifndef JSONARRAY_H
#define JSONARRAY_H

#include "JSONValue.h"
#include <vector>

namespace PJL {
	class JSONArray {
	public:
		void Add(const JSONValue& value);
		const JSONValue& Get(size_t index) const;
		size_t Size() const;

	private:
		std::vector<JSONValue> array;
	};

	inline JSONValue::JSONValue(const JSONArray& value) : data(std::make_shared<JSONArray>(value)) {};
}

#endif