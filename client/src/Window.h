#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif
#include <functional>
#include <vector>
#include "Settings.h"

// Forward declarations to avoid too many headers
class Direct3D;

class Window {
    public:
        std::function<void()> windowResizedHandler;
        std::function<void(char)> e_charTyped;
        std::function<void(char)> e_keyPressed;
        std::function<void(char)> e_keyReleased;
        std::function<void(int x, int y)> e_mouseMoved = [](int x, int y){};
        std::function<void(int key)> e_mouseButtonPressed;
        std::function<void(int key)> e_mouseButtonReleased;

        int width_;
        int height_;

        Window(int res_x, int res_y, WindowMode mode);
        ~Window();

        HWND GetWindowHandle();
            
        // Show window on screen
        void Show();
        // Handle window events once per frame if applicable.
        // Depending on platform implementation, this could trigger window resized, key presses, mouse movement and much more...
        void HandleEvents();
        // Will return true whenever the window wants to close or is already closed
        bool ShouldClose();
        // Set value of 'shouldClose' to true. Can not be reversed -> window WILL close and PMG WILL quit!
        void SetShouldClose();

        void FocusGained();
        void FocusLost();

        void ClipCursorToWindow();

        void Resized(int width_, int height_);

        void SetWindowMode(WindowMode new_mode, int resolution_x, int resolution_y);

        LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
            
    private:
        HWND windowHandle;
        bool shouldClose;
};
