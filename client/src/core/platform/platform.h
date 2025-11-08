#pragma once

#include <vector>
#include <core/platform/video_mode.h>

class CPlatform {
public:
    static std::vector<VideoMode_t> GetAllVideoModes();
};