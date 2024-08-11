project(PloinkysJSONLibrary)

add_library(PloinkysJSONLibrary
	Source/JSONArray.cpp
	Source/JSONObject.cpp
	Source/JSONParser.cpp
	Source/JSONValue.cpp
)
set_property(TARGET PloinkysJSONLibrary PROPERTY CXX_STANDARD 20)

target_include_directories(PloinkysJSONLibrary PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Public)
target_include_directories(PloinkysJSONLibrary PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Include)