#include "Window.h"
#include <windowsx.h>
#include <iostream>
#include <string>
#include "Direct3D.h"
#include "Mesh.h"
#include "logger.h"
#include "Resource.h"
#include "Settings.h"

LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    PMG::Window* window = (PMG::Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    
    if (window == nullptr) {
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }

    return window->WndProc(hwnd, msg, wParam, lParam);
}

namespace PMG {
    Window::Window(int res_x, int res_y, WindowMode mode) {
        Logger::Msg("Creating window...");

        this->width_ = res_x;
        this->height_ = res_y;

        LPCWSTR className = L"PloinkysMOBAGameWindow";

        shouldClose = false;

        HINSTANCE hInstance = GetModuleHandle(NULL);

        // ------ Create window -----
        WNDCLASSEXW wc{};
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.style = 0;
        wc.lpfnWndProc = StaticWndProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = hInstance;
        wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
        wc.hCursor = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_CURSOR1));
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wc.lpszMenuName = NULL;
        wc.lpszClassName = className;
        wc.hIconSm = wc.hIcon;

        int posX, posY;

        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);

        posX = (screenWidth - width_) / 2;
        posY = (screenHeight - height_) / 2;

        RegisterClassExW(&wc);

        DWORD dwStyle = WS_SYSMENU | WS_CAPTION;

        switch (mode) {
        case WindowMode::WINDOWED: {
            break;
        }
        case WindowMode::BORDERLESS:{
            dwStyle = WS_POPUP;
            width_ = screenWidth;
            height_ = screenHeight;
            posX = 0;
            posY = 0;
            break;
        }
        case WindowMode::FULLSCREEN:
        default: {
            dwStyle = WS_POPUP;
            width_ = screenWidth;
            height_ = screenHeight;
            posX = 0;
            posY = 0;
            break;
        }
        }

        RECT wr = {0, 0, width_, height_};       // set the size, but not the position
        AdjustWindowRectEx(&wr, dwStyle, false, WS_EX_APPWINDOW); // adjust the window's size

        std::wstring window_title = L"Ploinky's MOBA Game";

#ifdef _DEBUG
        window_title.append(L"__DEBUG BUILD__");
#endif

        windowHandle = CreateWindowExW(WS_EX_APPWINDOW,
                                       wc.lpszClassName,
                                       window_title.c_str(),
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
            RECT r;
            GetClientRect(hwnd, &r);
            Logger::Msg(std::string("WM_SIZE: ").append(std::to_string(r.right).append("-").append(std::to_string(r.bottom))));
            Resized(r.right, r.bottom);
            break;
        }
        case WM_CHAR: {
            e_charTyped(static_cast<UINT>(wParam));
            break;
        }
        case WM_KEYDOWN: {
            WORD ascii = 0;
            BYTE keyboardState[256];
            if (GetKeyboardState(keyboardState)) {
                const int keyboardScanCode = (lParam >> 16) & 0x00ff;
                ToAscii(static_cast<UINT>(wParam), keyboardScanCode, keyboardState, &ascii, 0);
                e_keyPressed(static_cast<char>(ascii));
            }
            break;
        }
        case WM_KEYUP: {
            WORD ascii = 0;
            BYTE keyboardState[256];
            if (GetKeyboardState(keyboardState)) {
                const int keyboardScanCode = (lParam >> 16) & 0x00ff;
                ToAscii(static_cast<UINT>(wParam), keyboardScanCode, keyboardState, &ascii, 0);
                e_keyReleased(static_cast<char>(ascii));
            }
            break;
        }
        case WM_MOUSEMOVE: {
            e_mouseMoved(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            break;
        }
        case WM_LBUTTONDOWN: {
            e_mouseButtonPressed(0);
            break;
        }
        case WM_MBUTTONDOWN: {
            e_mouseButtonPressed(1);
            break;
        }
        case WM_RBUTTONDOWN: {
            e_mouseButtonPressed(2);
            break;
        }
        case WM_LBUTTONUP: {
            e_mouseButtonReleased(0);
            break;
        }
        case WM_MBUTTONUP: {
            e_mouseButtonReleased(1);
            break;
        }
        case WM_RBUTTONUP: {
            e_mouseButtonReleased(2);
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
        default: {
            return DefWindowProcW(hwnd, msg, wParam, lParam);        
        }
        }

        return 0;
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

    void Window::Resized(int width_, int height_) {
        this->width_ = width_;
        this->height_ = height_;

        if(windowResizedHandler != nullptr) {
            windowResizedHandler();
        }
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
        
        POINT ul{};
        ul.x = rect.left;
        ul.y = rect.top;

        POINT lr{};
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

    void Window::SetWindowMode(WindowMode new_mode, int resolution_x, int resolution_y) {
        int posX, posY = 0;
        int new_width, new_height = 0;

        DWORD dwStyle = WS_SYSMENU | WS_CAPTION;

        switch (new_mode) {
            case WindowMode::WINDOWED: {
                new_width = resolution_x;
                new_height = resolution_y;
                posX = GetSystemMetrics(SM_CXSCREEN) / 2 - resolution_x / 2;
                posY = GetSystemMetrics(SM_CYSCREEN) / 2 - resolution_y / 2;
                break;
            }
            case WindowMode::BORDERLESS: {
                dwStyle = WS_POPUP;
                new_width = GetSystemMetrics(SM_CXSCREEN);
                new_height = GetSystemMetrics(SM_CYSCREEN);
                posX = 0;
                posY = 0;
                break;
            }
            case WindowMode::FULLSCREEN:
            default: {
                dwStyle = WS_POPUP;
                new_width = GetSystemMetrics(SM_CXSCREEN);
                new_height = GetSystemMetrics(SM_CYSCREEN);
                posX = 0;
                posY = 0;
                break;
            }
        }

        SetWindowLongPtr(windowHandle, GWL_STYLE, dwStyle);
        Logger::Msg(std::string("setting new mode: ").append(std::to_string(static_cast<int>(new_mode))).append(";").append(std::to_string(new_width).append("-").append(std::to_string(new_height))));
        SetWindowPos(windowHandle, NULL, posX, posY, new_width, new_height, SWP_SHOWWINDOW | SWP_FRAMECHANGED);

        ClipCursorToWindow();
    }
}