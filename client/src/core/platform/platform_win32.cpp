#include <core/platform/platform.h>

std::vector<VideoMode_t> CPlatform::GetAllVideoModes() {
    std::vector<VideoMode_t> deviceModes;

	DEVMODEA devMode{};
	devMode.dmSize = sizeof(DEVMODE);
	for (int i = 0; EnumDisplaySettingsA(NULL, i, &devMode) != 0; i++) {
        VideoMode_t vidMode{};
        vidMode.uWidth = devMode.dmPelsWidth;
        vidMode.uHeight = devMode.dmPelsHeight;
        deviceModes.emplace(vidMode);
	}

    return deviceModes;
}