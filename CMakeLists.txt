cmake_minimum_required(VERSION 3.27)

project(PloinkysMOBAGame)

set(CMAKE_CXX_STANDARD 20)

link_directories($(CMAKE_SOURCE_DIR)/Bin)

enable_testing()
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.15.0.zip
)
FetchContent_MakeAvailable(googletest)


set(TEMP_BUILD_DIR ${CMAKE_SOURCE_DIR}/Temp)
set(DIST_BUILD_DIR ${CMAKE_SOURCE_DIR}/Dist)

add_subdirectory(Lib)
add_subdirectory(App)
add_subdirectory(Vendor)