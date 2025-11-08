#pragma once

#include <vector>
#include <core/platform/video_mode.h>
#include <core/platform/cursor-id.h>

class CPlatform {
public:
    static std::vector<VideoMode_t> GetAllVideoModes();
    static void RequestCursor(CursorId cursor);
};