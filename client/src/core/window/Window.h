#pragma once

#include <functional>
#include <vector>
#include <memory>

#ifdef WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <core/input/key-code.h>

#include <core/window/h-window.h>
#include "Settings.h"

// Forward declarations to avoid too many headers
class CD3D11GraphicsEngine;

class Window {
    public:
        std::function<void()> windowResizedHandler;
        std::function<void(uint32_t)> e_charTyped;
        std::function<void(EKeyCode)> e_keyPressed;
        std::function<void(EKeyCode)> e_keyReleased;
        std::function<void(int x, int y)> e_mouseMoved = [](int x, int y){};
        std::function<void(EMouseButton)> e_mouseButtonPressed;
        std::function<void(int key)> e_mouseButtonReleased;

        int width_;
        int height_;

        Window(int res_x, int res_y, WindowMode mode);
        ~Window();

        HWindow GetWindowHandle();
            
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
            
    private:
        GLFWwindow* m_pWindow = nullptr;

        bool shouldClose;
};
