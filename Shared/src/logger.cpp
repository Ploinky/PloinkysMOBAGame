#include "logger.h"

#ifdef _DEBUG
#include <windows.h> 
#endif

namespace PMG {
    void Logger::Msg(std::string message) {
#ifdef _DEBUG
        OutputDebugStringA(message.append("\r\n").c_str());
#endif

#ifdef NDEBUG
        std::cout << message << std::endl;
#endif
    }

    void Logger::WMsg(std::wstring wmessage) {
#ifdef _DEBUG
        OutputDebugString(wmessage.append(L"\r\n").c_str());
#endif

#ifdef NDEBUG
        std::wcout << wmessage << std::endl;
#endif
    }
    
    void Logger::Err(std::string error) {
#ifdef _DEBUG
        OutputDebugStringA(error.append("\r\n").c_str());
#endif

#ifdef NDEBUG
        std::cerr << error << std::endl;
#endif
    }
    
    void Logger::WErr(std::wstring werror) {
#ifdef _DEBUG
        OutputDebugString(werror.append(L"\r\n").c_str());
#endif

#ifdef NDEBUG
        std::wcerr << werror << std::endl;
#endif
    }
}