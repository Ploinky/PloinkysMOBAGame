#include <core/platform/platform.h>
#include "../Resources/resource.h"
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

bool CPlatform::Initialize() {
    // Initialize COM?
    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    return FAILED(hr)
}
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

void CPlatform::Alert(std::string strTitle, std::string strText, EAlertType eType) {
    int nIcon = MB_ICONERROR;

    switch(eType) {
        case EAlertType::INFO:
            nIcon = MB_ICONINFO;
            break;
        case EAlertType::ERROR:
        default:
            break;
    }

    MessageBoxA(nullptr, strText.c_str(), strText.c_str(), MB_ICONERROR);
}


void CPlatform::ShowConsole() {
    AllocConsole();
    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
    freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
}