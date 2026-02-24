add_executable(nav_vis WIN32
        src/main.cpp
        src/world-map.cpp
)
target_link_libraries(nav_vis PUBLIC common)