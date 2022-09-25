#include "window.hpp"
#include <windowsx.h>
#include <iostream>
#include <string>
#include "direct3d.hpp"
#include "mesh.hpp"
#include "Shared/code/logger.hpp"

LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    PMG::Window* window = (PMG::Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    
    if (window == nullptr) {
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }

    return window->WndProc(hwnd, msg, wParam, lParam);
}

namespace PMG {
    Window::Window() {
        Logger::Msg("Creating window...");

        LPCWSTR className = L"PloinkysMOBAGameWindow";

        shouldClose = false;

        // ------ Create window -----
        WNDCLASSEXW wc;
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        wc.lpfnWndProc = StaticWndProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = GetModuleHandle(NULL);
        wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wc.lpszMenuName = NULL;
        wc.lpszClassName = className;
        wc.hIconSm = wc.hIcon;

        int posX, posY;
        width = 1024;
        height = 800;

        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);

        posX = (screenWidth - width) / 2;
        posY = (screenHeight - height) / 2;

        RegisterClassExW(&wc);

        DWORD dwStyle = WS_OVERLAPPEDWINDOW;

        RECT wr = {0, 0, width, height};       // set the size, but not the position
        AdjustWindowRect(&wr, dwStyle, FALSE); // adjust the window's size
        width = wr.right - wr.left;
        height = wr.bottom - wr.top;

        LPCWSTR winTitle = L"Ploinky's MOBA Game";

        windowHandle = CreateWindowExW(WS_EX_APPWINDOW,
                                       wc.lpszClassName,
                                       winTitle,
                                       dwStyle,
                                       posX,
                                       posY,
                                       wr.right - wr.left,
                                       wr.bottom - wr.top,
                                       NULL,
                                       NULL,
                                       GetModuleHandle(NULL),
                                       NULL);
        
        // Save pointer to PMG::Window for WndProc to access
        SetWindowLongPtr(windowHandle, GWLP_USERDATA, LONG_PTR(this));

        if (windowHandle == 0) {
            Logger::Err("WindowHandle is NULL.");
            Logger::Err(std::to_string(GetLastError()));
            return;
        }
    }

    Window::~Window() {
    }

    LRESULT CALLBACK Window::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        switch (msg) {
        case WM_QUIT:
        case WM_DESTROY: {
            SetShouldClose();
            break;
        }
        case WM_SIZE: {
            Resized(LOWORD(lParam), HIWORD(lParam));
            break;
        }
        case WM_KEYDOWN: {
            m_keys[wParam] = true;
            break;
        }
        case WM_KEYUP: {
            m_keys[wParam] = false;
            break;
        }
        case WM_MOUSEMOVE: {
            m_mouseX = GET_X_LPARAM(lParam);
            m_mouseY = GET_Y_LPARAM(lParam);
            break;
        }
        case WM_LBUTTONDOWN: {
            m_mouseBtnDown[0] = true;
            break;
        }
        case WM_MBUTTONDOWN: {
            m_mouseBtnDown[1] = true;
            break;
        }
        case WM_RBUTTONDOWN: {
            m_mouseBtnDown[2] = true;
            break;
        }
        case WM_LBUTTONUP: {
            m_mouseBtnDown[0] = false;
            break;
        }
        case WM_MBUTTONUP: {
            m_mouseBtnDown[1] = false;
            break;
        }
        case WM_RBUTTONUP: {
            m_mouseBtnDown[2] = false;
            break;
        }
        case WM_ACTIVATE: {
            /*
            PMG::Window *window = (PMG::Window *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
            if(window == nullptr) {
                break;
            }

            if(LOWORD(wParam) == WA_INACTIVE) {
                window->FocusLost();
            } else if(LOWORD(wParam) == WA_ACTIVE) {
                window->FocusGained();
            }
            break;
            */
        }
        case WM_SETFOCUS: {
            PMG::Window* window = (PMG::Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
            if (window == nullptr) {
                break;
            }

            window->FocusGained();
            break;
        }
        case WM_KILLFOCUS: {
            PMG::Window* window = (PMG::Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
            if (window == nullptr) {
                break;
            }

            window->FocusLost();
            break;
        }
        }

        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }


    void Window::Show() {
        Logger::Msg("Showing window....");
        ShowWindow(windowHandle, SW_SHOW);
    }

    MSG msg = {};
    
    // Handle win32 window events
    void Window::HandleEvents() {
        // Use PeekMessage, GetMessage blocks!
        while (PeekMessage(&msg, windowHandle, 0, 0, PM_REMOVE) > 0) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    bool Window::ShouldClose() {
        return shouldClose;
    }

    void Window::SetShouldClose() {
        shouldClose = true;
    }
    
    HWND Window::GetWindowHandle() {
        return windowHandle;
    }

    void Window::Resized(int width, int height) {
        this->width = width;
        this->height = height;

        if(windowResizedHandler != nullptr) {
            windowResizedHandler();
        }
    }


    short Window::GetMouseX() {
        return m_mouseX;
    }
    
    short Window::GetMouseY() {
        return m_mouseY;
    }

    bool Window::IsButtonDown(int button) {
        return m_mouseBtnDown[button];
    }

    bool Window::IsKeyDown(char key) {
        return m_keys[key];
    }

    void Window::FocusGained() {
        ClipCursorToWindow();
    }

    void Window::FocusLost() {
        // Do not forget to unclip cursor!
        ClipCursor(NULL);
    }

    void Window::ClipCursorToWindow() {
        RECT rect;
        GetClientRect(windowHandle, &rect);
        
        POINT ul;
        ul.x = rect.left;
        ul.y = rect.top;

        POINT lr;
        lr.x = rect.right;
        lr.y = rect.bottom;

        MapWindowPoints(windowHandle, nullptr, &ul, 1);
        MapWindowPoints(windowHandle, nullptr, &lr, 1);

        rect.left = ul.x;
        rect.top = ul.y;

        rect.right = lr.x;
        rect.bottom = lr.y;

        ClipCursor(&rect);
    }
}