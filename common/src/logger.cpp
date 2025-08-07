#include "logger.h"

#ifdef _DEBUG
#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
#endif
#endif

#include <chrono>

std::string Logger::PrependTimeAndDate(std::string str) {
    std::time_t time = std::time(nullptr);
    std::tm* localTime = std::localtime(&time);

    char* buf = (char*) malloc(26 * sizeof(char));

    sprintf_s(buf, 26, "[%d-%02d-%02d] [%02d:%02d:%02d]: ",
        localTime->tm_year + 1900, localTime->tm_mon + 1, localTime->tm_mday, localTime->tm_hour, localTime->tm_min, localTime->tm_sec
    );

    return std::string(buf).append(str);
}

void Logger::Msg(std::string message) {
    message = PrependTimeAndDate(message).append("\n");
    
#ifdef _DEBUG
    OutputDebugStringA(message.c_str());
#else
    std::cout << message;
#endif
}

void Logger::WMsg(std::wstring wmessage) {
    wmessage.append(L"\n");

#ifdef _DEBUG
    OutputDebugStringW(wmessage.c_str());
#else
    std::wcout << wmessage;
#endif
}

void Logger::Err(std::string error) {
    error = PrependTimeAndDate(error).append("\n");

#ifdef _DEBUG
    OutputDebugStringA(error.c_str());
#else
    std::cerr << error;
#endif
}

void Logger::WErr(std::wstring werror) {
    werror.append(L"\n");

#ifdef _DEBUG
    OutputDebugStringW(werror.c_str());
#else
    std::wcerr << werror;
#endif
}