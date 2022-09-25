#include "logger.hpp"

#ifdef _DEBUG
#include <windows.h> 
#endif

namespace PMG {
    void Logger::Msg(std::string message) {
#ifdef _DEBUG
        OutputDebugStringA(message.c_str());
#endif

#ifdef NDEBUG
        std::cout << message << std::endl;
#endif
    }

    void Logger::WMsg(std::wstring wmessage) {
#ifdef _DEBUG
        OutputDebugString(wmessage.c_str());
#endif

#ifdef NDEBUG
        std::wcout << wmessage << std::endl;
#endif
    }
    
    void Logger::Err(std::string error) {
#ifdef _DEBUG
        OutputDebugStringA(error.c_str());
#endif

#ifdef NDEBUG
        std::cerr << error << std::endl;
#endif
    }
    
    void Logger::WErr(std::wstring werror) {
#ifdef _DEBUG
        OutputDebugString(werror.c_str());
#endif

#ifdef NDEBUG
        std::wcerr << werror << std::endl;
#endif
    }
}