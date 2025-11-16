#pragma once

#include <core/window/Window.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

class CWindowWin32 : public Window {
public:
    CWindowWin32(int res_x, int res_y, WindowMode mode);
    ~CWindowWin32();

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

    LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
    HWND m_hWnd = nullptr;
    HBRUSH m_hBrushBackground;

};