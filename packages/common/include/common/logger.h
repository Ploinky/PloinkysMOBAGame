#pragma once

#include <iostream>
#include <string>

class Logger {
    public:
        static void Msg(std::string message);
        static void WMsg(std::wstring wmessage);
        static void Err(std::string error);
        static void WErr(std::wstring werror);

        template< typename... Args >
        static void FormatMsg(const char* format, Args... args) {
            int length = std::snprintf(nullptr, 0, format, args...);

            if (length < 0) {
                return;
            }

            char* buf = new char[length + 1];
            std::snprintf(buf, length + 1, format, args...);

            Msg(buf);

            delete buf;
        }

        template< typename... Args >
        static void FormatErr(const char* format, Args... args) {
            int length = std::snprintf(nullptr, 0, format, args...);

            if (length < 0) {
                return;
            }

            char* buf = new char[length + 1];
            std::snprintf(buf, length + 1, format, args...);

            Err(buf);

            delete buf;
        }

private:
        static std::string PrependTimeAndDate(std::string str);
};