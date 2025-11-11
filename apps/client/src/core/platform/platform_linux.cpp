#include <core/platform/platform.h>

#include <common/logger.h>

std::vector<VideoMode_t> CPlatform::GetAllVideoModes() {
    return {};
}

bool CPlatform::Initialize() {
    return true;
}

void CPlatform::RequestCursor(CursorId cursor) {

}

void CPlatform::Alert(std::string strTitle, std::string strText, EAlertType eType) {
    switch(eType) {
        case EAlertType::INFO:
            Logger::FormatMsg("%s - %s", strTitle.c_str(), strText.c_str());
        case EAlertType::ERR:
        default:
            Logger::FormatErr("%s - %s", strTitle.c_str(), strText.c_str());
    }
}

void CPlatform::ShowConsole() {

}