#pragma once

#include <iostream>
#include <string>

namespace P3D {
    class Logger {
        public:
            static void Msg(std::string message);
            static void WMsg(std::wstring wmessage);
            static void Err(std::string error);
            static void WErr(std::wstring werror);
    };
}