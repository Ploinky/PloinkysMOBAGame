add_library(paklib
    src/pak-file.cpp
)

target_include_directories(paklib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE src
)

set_target_properties(paklib PROPERTIES
    DEBUG_POSTFIX d
)