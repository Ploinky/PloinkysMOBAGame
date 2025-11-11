# current version at the time this project was created...
cmake_minimum_required(VERSION 3.28)

# set the standard the same for all projects
set(CMAKE_CXX_STANDARD 20)

# global distributable directory
set_property(GLOBAL PROPERTY dist_dir ${CMAKE_SOURCE_DIR}/"_dist")
set_property(GLOBAL PROPERTY out_dir ${CMAKE_SOURCE_DIR}/"_out")

# here we go now
project(PloinkysMOBAGame)

# add projects, common first since the others need it'
add_subdirectory(common)
add_subdirectory(server)
add_subdirectory(client)
add_subdirectory(test)