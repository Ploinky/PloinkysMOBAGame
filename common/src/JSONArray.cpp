#include "JSONArray.h"

namespace PJL {
	size_t JSONArray::Size() const {
		return array.size();
	}
	
	const JSONValue& JSONArray::Get(size_t index) const {
		return array.at(index);
	}
	
	void JSONArray::Add(const JSONValue& value) {
		array.push_back(value);
	}
}