#pragma once

#include <core/window/Window.h>

#define GLFW_EXPOSE_NATIVE_LINUX
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

class CWindowGLFW : public Window {
public:
    CWindowGLFW(int res_x, int res_y, WindowMode mode);
    ~CWindowGLFW();

    // Show window on screen
    virtual void Show() override;
    // Handle window events once per frame if applicable.
    // Depending on platform implementation, this could trigger window resized, key presses, mouse movement and much more...
    virtual void HandleEvents() override;

    virtual void FocusGained() override;
    virtual void FocusLost() override;

    virtual void ClipCursorToWindow() override;

    virtual void Resized(int width_, int height_) override;

    virtual void SetWindowMode(WindowMode new_mode, int resolution_x, int resolution_y) override;
    
    virtual HWindow GetWindowHandle() override;
private:
    GLFWwindow* m_pWindow = nullptr;
};