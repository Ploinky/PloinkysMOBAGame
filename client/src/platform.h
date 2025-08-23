#pragma once

#ifdef _WIN32
#define _WINSOCKAPI_     // prevent windows.h from including winsock.h
#include <winsock2.h>
#include <windows.h>
#endif