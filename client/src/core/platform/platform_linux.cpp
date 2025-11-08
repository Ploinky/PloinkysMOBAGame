#include <core/platform/platform.h>

std::vector<VideoMode_t> CPlatform::GetAllVideoModes() {
    return {};
}

bool CPlatform::Initialize() {
    return true;
}

void CPlatform::RequestCursor(CursorId cursor) {

}

void CPlatform::Alert(std::string strTitle, std::string strText, EAlertType eType) {

}

void CPlatform::ShowConsole() {

}