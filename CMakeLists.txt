# current version at the time this project was created...
cmake_minimum_required(VERSION 3.28)

# set the standard the same for all projects
set(CMAKE_CXX_STANDARD 20)

# here we go now
project(PloinkysMOBAGame)

# add projects, common first since the others need it'
add_subdirectory(common)
add_subdirectory(assets)
add_subdirectory(server)
add_subdirectory(client)