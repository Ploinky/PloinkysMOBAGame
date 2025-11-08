#pragma once

#include <vector>
#include <string>

#include <core/platform/video_mode.h>
#include <core/platform/cursor-id.h>

enum class EAlertType {
    ERROR,
    INFO
};

class CPlatform {
public:
    static bool Initialize();
    static std::vector<VideoMode_t> GetAllVideoModes();
    static void RequestCursor(CursorId cursor);
    static void Alert(std::string strTitle, std::string strText, EAlertType eType);
    static void ShowConsole();
};