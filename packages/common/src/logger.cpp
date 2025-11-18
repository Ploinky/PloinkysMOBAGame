#include "logger.h"

#ifdef _DEBUG
#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
#endif
#endif

#include <chrono>

std::string Logger::PrependTimeAndDate(std::string str) {
    std::chrono::time_point tpNow = std::chrono::high_resolution_clock::now();
    std::time_t time = std::time(nullptr);
    std::tm* localTime = std::localtime(&time);
    auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(tpNow);
    auto fraction = tpNow - seconds;
    const std::chrono::duration<double> tse = tpNow.time_since_epoch();
    std::chrono::seconds::rep milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(tse).count() % 1000;



    char* buf = (char*) malloc(26 * sizeof(char));

    snprintf(buf, 30, "[%d-%02d-%02d] [%02d:%02d:%02d.%03llu]: ",
        localTime->tm_year + 1900, localTime->tm_mon + 1, localTime->tm_mday, localTime->tm_hour, localTime->tm_min, localTime->tm_sec, milliseconds
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