# current version at the time this project was created...
cmake_minimum_required(VERSION 3.28)

# set the standard the same for all projects
set(CMAKE_CXX_STANDARD 20)

# global distributable directory
set_property(GLOBAL PROPERTY dist_dir ${CMAKE_SOURCE_DIR}/${CMAKE_DIST_FOLDER})
set_property(GLOBAL PROPERTY out_dir ${CMAKE_SOURCE_DIR}/${CMAKE_OUT_FOLDER})

# here we go now
project(PloinkysMOBAGame)

# add projects, common first since the others need it'
add_subdirectory(packages/common)
add_subdirectory(packages/paklib)
add_subdirectory(apps/server)
add_subdirectory(apps/client)
add_subdirectory(apps/nav_vis)
add_subdirectory(apps/pakr)

add_subdirectory(test/nav_test)
add_subdirectory(test/server_test)