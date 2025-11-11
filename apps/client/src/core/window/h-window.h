#pragma once

#ifdef WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    typedef HWND HWindow;
#else
    #include <GLFW/glfw3.h>
    typedef GLFWwindow* HWindow;
#endif
