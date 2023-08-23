#pragma once

#include <string>
#include <vector>
#include <cstddef>
#include <list>
#include <codecvt>

namespace PMG {
    struct file_t {
        unsigned char* data;
        size_t size;
    };

    class Util {
        public:
            static file_t ReadBytesFromFile(std::string file);
            static std::list<std::string> ReadLinesFromFile(std::string fileName);
            static void WriteLinesToFile(std::string fileName, std::list<std::string> content);
	        static std::list<std::string> SplitString(std::string str, std::string delim);
            static std::list<std::string> SplitString(std::string str, char delim);
            static long long GetSystemTime();

            static std::wstring string_to_wstring(const std::string& str)
            {
                using convert_typeX = std::codecvt_utf8<wchar_t>;
                std::wstring_convert<convert_typeX, wchar_t> converterX;

                return converterX.from_bytes(str);
            }

            static std::string wstring_to_string(const std::wstring& wstr)
            {
                using convert_typeX = std::codecvt_utf8<wchar_t>;
                std::wstring_convert<convert_typeX, wchar_t> converterX;

                return converterX.to_bytes(wstr);
            }

    };
}