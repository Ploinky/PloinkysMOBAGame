#pragma once

#ifdef WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    typedef HWND HWindow;
#else
    typedef void* HWindow;
#endif
