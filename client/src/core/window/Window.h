#pragma once

#include <functional>
#include <vector>
#include <memory>
#include <core/input/key-code.h>
#include <core/window/h-window.h>
#include "Settings.h"

// Forward declarations to avoid too many headers
class CD3D11GraphicsEngine;

class Window {
public:
    static Window* Create(int res_x, int res_y, WindowMode mode);
    std::function<void()> windowResizedHandler;
    std::function<void(uint32_t)> e_charTyped;
    std::function<void(EKeyCode)> e_keyPressed;
    std::function<void(EKeyCode)> e_keyReleased;
    std::function<void(int x, int y)> e_mouseMoved = [](int x, int y){};
    std::function<void(EMouseButton)> e_mouseButtonPressed;
    std::function<void(EMouseButton)> e_mouseButtonReleased;

    int width_;
    int height_;

    virtual ~Window() = default;

    virtual HWindow GetWindowHandle() = 0;
        
    // Show window on screen
    virtual void Show() = 0;
    // Handle window events once per frame if applicable.
    // Depending on platform implementation, this could trigger window resized, key presses, mouse movement and much more...
    virtual void HandleEvents() = 0;
    // Will return true whenever the window wants to close or is already closed
    inline bool ShouldClose() { return shouldClose; };
    // Set value of 'shouldClose' to true. Can not be reversed -> window WILL close and PMG WILL quit!
    inline void SetShouldClose() { shouldClose = true; };

    virtual void FocusGained() = 0;
    virtual void FocusLost() = 0;

    virtual void ClipCursorToWindow() = 0;

    virtual void Resized(int width_, int height_) = 0;

    virtual void SetWindowMode(WindowMode new_mode, int resolution_x, int resolution_y) = 0;

    protected:

    bool shouldClose;
};
