#include "logger.h"

#ifdef _DEBUG
#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
#endif
#endif

namespace PMG {
    void Logger::Msg(std::string message) {
        message.append("\n");

#ifdef _DEBUG
        OutputDebugStringA(message.c_str());
#endif

        std::cout << message;
    }

    void Logger::WMsg(std::wstring wmessage) {
        wmessage.append(L"\n");

#ifdef _DEBUG
        OutputDebugString(wmessage.c_str());
#endif

        std::wcout << wmessage;
    }

    void Logger::Err(std::string error) {
        error.append("\n");

#ifdef _DEBUG
        OutputDebugStringA(error.c_str());
#endif

        std::cerr << error;
    }

    void Logger::WErr(std::wstring werror) {
        werror.append(L"\n");

#ifdef _DEBUG
        OutputDebugString(werror.c_str());
#endif

        std::wcerr << werror;
    }
}