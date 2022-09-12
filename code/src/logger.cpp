#include "logger.hpp"

namespace P3D {
    void Logger::Msg(std::string message) {
        std::cout << message << std::endl;
    }

    void Logger::WMsg(std::wstring wmessage) {
        std::wcout << wmessage << std::endl;
    }
    
    void Logger::Err(std::string error) {
        std::cerr << error << std::endl;
    }
    
    void Logger::WErr(std::wstring werror) {
        std::wcerr << werror << std::endl;
    }
}