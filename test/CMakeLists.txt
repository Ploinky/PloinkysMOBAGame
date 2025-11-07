file(GLOB_RECURSE SERVER_SOURCES ${CMAKE_SOURCE_DIR}/server/src/**.cpp)
list(REMOVE_ITEM SERVER_SOURCES ${CMAKE_SOURCE_DIR}/server/src/Main.cpp)


add_executable(test
    src/main.cpp

    src/server/test-server.cpp
    src/common/test-matrix.cpp

    ${SERVER_SOURCES}
)

target_include_directories(test
  PRIVATE
  ${CMAKE_SOURCE_DIR}/common/include
  ${CMAKE_SOURCE_DIR}/server/src
  ${CMAKE_SOURCE_DIR}/vendor/include
  src
)

target_link_directories(test PRIVATE ${CMAKE_SOURCE_DIR}/vendor/lib)
target_link_libraries(test
  PRIVATE
  common
  steam_api64
)
