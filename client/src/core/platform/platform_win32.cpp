#include <core/platform/platform.h>
#include "../Resources/resource.h"
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif
std::vector<VideoMode_t> CPlatform::GetAllVideoModes() {
    std::vector<VideoMode_t> deviceModes;

	DEVMODEA devMode{};
	devMode.dmSize = sizeof(DEVMODE);
	for (int i = 0; EnumDisplaySettingsA(NULL, i, &devMode) != 0; i++) {
        VideoMode_t vidMode{};
        vidMode.uWidth = devMode.dmPelsWidth;
        vidMode.uHeight = devMode.dmPelsHeight;
        vidMode.uColorDepth = devMode.dmBitsPerPel;
        deviceModes.push_back(vidMode);
	}

    return deviceModes;
}

void CPlatform::RequestCursor(CursorId idCursor) {
    switch (idCursor) {
    case CursorId::BUTTON_HOVER: {
        SetCursor(LoadCursor(GetModuleHandleA(NULL), MAKEINTRESOURCE(IDC_HOVER_BUTTON)));
        break;
    }
    case CursorId::ATTACK_MOVE: {
        SetCursor(LoadCursor(GetModuleHandleA(NULL), MAKEINTRESOURCE(IDC_ATTACK)));
        break;
    }
    case CursorId::DEFAULT:
    default: {
        SetCursor(LoadCursor(GetModuleHandleA(NULL), MAKEINTRESOURCE(IDC_DEFAULT)));
    }
    }
}