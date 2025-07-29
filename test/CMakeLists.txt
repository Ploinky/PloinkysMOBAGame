file(GLOB_RECURSE SERVER_SOURCES ${CMAKE_SOURCE_DIR}/server/src/**.cpp)
list(REMOVE_ITEM SERVER_SOURCES ${CMAKE_SOURCE_DIR}/server/src/Main.cpp)

link_directories($(CMAKE_SOURCE_DIR)/Bin ${CMAKE_SOURCE_DIR}/vendor/steam/redistributable_bin/win64)

add_executable(test
    src/main.cpp

    src/server/test-server.cpp

    ${SERVER_SOURCES}
)

target_include_directories(test
  PRIVATE
  ${CMAKE_SOURCE_DIR}/common/include
  ${CMAKE_SOURCE_DIR}/server/src
  ${CMAKE_SOURCE_DIR}/vendor/steam/public
  src
)

target_link_libraries(test
  PRIVATE
  common
  steam_api64
)