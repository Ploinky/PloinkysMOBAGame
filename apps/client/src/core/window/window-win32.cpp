#include "window-win32.h"
#include <iostream>
#include <string>
#include <common/PMG_Common.h>
#include "../Resources/resource.h"
#include "Settings.h"
#include <map>
#include <windowsx.h>

static const std::map<uint64_t, EKeyCode> g_mapKeyCode = {
    {VK_ESCAPE, EKeyCode::ESCAPE},
    {VK_SPACE, EKeyCode::SPACE},
    {'a', EKeyCode::A},
    {'b', EKeyCode::B},
    {'c', EKeyCode::C},
    {'d', EKeyCode::D},
    {'e', EKeyCode::E},
    {'f', EKeyCode::F},
    {'g', EKeyCode::G},
    {'h', EKeyCode::H},
    {'i', EKeyCode::I},
    {'j', EKeyCode::J},
    {'k', EKeyCode::K},
    {'l', EKeyCode::L},
    {'m', EKeyCode::M},
    {'n', EKeyCode::N},
    {'o', EKeyCode::O},
    {'p', EKeyCode::P},
    {'q', EKeyCode::Q},
    {'r', EKeyCode::R},
    {'s', EKeyCode::S},
    {'t', EKeyCode::T},
    {'y', EKeyCode::U},
    {'v', EKeyCode::V},
    {'w', EKeyCode::W},
    {'x', EKeyCode::X},
    {'y', EKeyCode::Y},
    {'z', EKeyCode::Z},
    {VK_LEFT, EKeyCode::LEFT_ARROW},
    {VK_RIGHT, EKeyCode::RIGHT_ARROW},
    {VK_UP, EKeyCode::UP_ARROW},
    {VK_DOWN, EKeyCode::DOWN_ARROW},
    {'1', EKeyCode::NUM1}
};

Window* Window::Create(int res_x, int res_y, WindowMode mode) {
    return new CWindowWin32(res_x, res_y, mode);
}

/*
void GLFWKeyCallback(GLFWwindow* pWindow, int nKey, int nScanCode, int nAction, int nMods) {
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(pWindow));

    if (nAction == GLFW_PRESS) {
        if(g_mapKeyCode.contains(nKey)) {
            self->e_keyPressed(g_mapKeyCode.at(nKey));
        }
    } else if(nAction == GLFW_RELEASE) {
        if(g_mapKeyCode.contains(nKey)) {
            self->e_keyReleased(g_mapKeyCode.at(nKey));
        }
    }
}

void GLFWMouseButtonCallback(GLFWwindow* pWindow, int nButton, int nAction, int nMods) {
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(pWindow));
    if (nAction == GLFW_PRESS) {
        if(nButton == GLFW_MOUSE_BUTTON_1) {
            self->e_mouseButtonPressed(EMouseButton::LEFT);
        }
        if(nButton == GLFW_MOUSE_BUTTON_2) {
            self->e_mouseButtonPressed(EMouseButton::RIGHT);
        }
    } else if (nAction == GLFW_RELEASE) {
        self->e_mouseButtonReleased(nButton);
    } 
} 
 
void GLFWMouseMoveCallback(GLFWwindow* pWindow, double dXPos, double dYPos) {
    CWindowWin32* self = static_cast<CWindowWin32*>(glfwGetWindowUserPointer(pWindow));
    self->e_mouseMoved(dXPos, dYPos);
}

void GLFWWindowFocusCallback(GLFWwindow* pWindow, int nFocused) {
    CWindowWin32* self = static_cast<CWindowWin32*>(glfwGetWindowUserPointer(pWindow));

    if(nFocused) {
        self->FocusGained();
    } else {
        self->FocusLost();
    }
}
*/

LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    CWindowWin32* window = (CWindowWin32*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        
    if (window == nullptr) {
        return DefWindowProcA(hwnd, msg, wParam, lParam);
    }

    return window->WndProc(hwnd, msg, wParam, lParam);
}

CWindowWin32::CWindowWin32(int res_x, int res_y, WindowMode mode) {
    Logger::Msg("Creating window...");

    this->width_ = res_x;
    this->height_ = res_y;

    LPCSTR className = "PloinkysMOBAGameWindow";

    shouldClose = false;

    HINSTANCE hInstance = GetModuleHandle(NULL);

    m_hBrushBackground = CreateSolidBrush(RGB(0, 0, 0));

    // ------ Create window -----
    WNDCLASSEXA wc{};
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.style = 0;
    wc.lpfnWndProc = StaticWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wc.hCursor = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_DEFAULT));
    wc.hbrBackground = m_hBrushBackground;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = className;
    wc.hIconSm = wc.hIcon;

    int posX, posY;

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    posX = (screenWidth - width_) / 2;
    posY = (screenHeight - height_) / 2;

    RegisterClassExA(&wc);

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

    std::string window_title = "Ploinky's MOBA Game";

#ifdef _DEBUG
    window_title.append("__DEBUG BUILD__");
#endif

    m_hWnd = CreateWindowExA(WS_EX_APPWINDOW,
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
        
    // Save pointer to Window for WndProc to access
    SetWindowLongPtr(m_hWnd, GWLP_USERDATA, LONG_PTR(this));

    if (m_hWnd == 0) {
        Logger::Err("WindowHandle is NULL.");
        Logger::Err(std::to_string(GetLastError()));
        return;
    }
}

CWindowWin32::~CWindowWin32() {
}

LRESULT CALLBACK CWindowWin32::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
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
            
            if(g_mapKeyCode.contains(ascii)) {
                e_keyPressed(g_mapKeyCode.at(ascii));
            }
        }
        break;
    }
    case WM_KEYUP: {
        WORD ascii = 0;
        BYTE keyboardState[256];
        if (GetKeyboardState(keyboardState)) {
            const int keyboardScanCode = (lParam >> 16) & 0x00ff;
            ToAscii(static_cast<UINT>(wParam), keyboardScanCode, keyboardState, &ascii, 0);

            if(g_mapKeyCode.contains(ascii)) {
                e_keyReleased(g_mapKeyCode.at(ascii));
            }
        }
        break;
    }
    case WM_MOUSEMOVE: {
        e_mouseMoved(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        break;
    }
    case WM_LBUTTONDOWN: {
        e_mouseButtonPressed(EMouseButton::LEFT);
        break;
    }
    case WM_MBUTTONDOWN: {
        e_mouseButtonPressed(EMouseButton::MIDDLE);
        break;
    }
    case WM_RBUTTONDOWN: {
        e_mouseButtonPressed(EMouseButton::RIGHT);
        break;
    }
    case WM_LBUTTONUP: {
        e_mouseButtonReleased(EMouseButton::LEFT);
        break;
    }
    case WM_MBUTTONUP: {
        e_mouseButtonReleased(EMouseButton::MIDDLE);
        break;
    }
    case WM_RBUTTONUP: {
        e_mouseButtonReleased(EMouseButton::RIGHT);
        break;
    }
    case WM_ACTIVATE: {
        Window *window = (Window *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        if(window == nullptr) {
            break;
        }

        if(LOWORD(wParam) == WA_INACTIVE) {
            window->FocusLost();
        } else if(LOWORD(wParam) == WA_ACTIVE) {
            window->FocusGained();
        }
        break;
    }
    case WM_SETFOCUS: {
        Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        if (window == nullptr) {
            break;
        }

        window->FocusGained();
        break;
    }
    case WM_KILLFOCUS: {
        Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        if (window == nullptr) {
            break;
        }

        window->FocusLost();
        break;
    }
    default: {
        return DefWindowProcA(hwnd, msg, wParam, lParam);        
    }
    }

    return 0;
}


void CWindowWin32::Show() {
    Logger::Msg("Showing window....");

    ShowWindow(m_hWnd, SW_SHOW);
}

// Handle win32 window events
void CWindowWin32::HandleEvents() {
    // Use PeekMessage, GetMessage blocks!
    MSG msg = {};
    while (PeekMessageA(&msg, m_hWnd, 0, 0, PM_REMOVE) > 0) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
}

HWindow CWindowWin32::GetWindowHandle() {
    return m_hWnd;
}

void CWindowWin32::Resized(int width_, int height_) {
    this->width_ = width_;
    this->height_ = height_;

    if(windowResizedHandler != nullptr) {
        windowResizedHandler();
    }
}

void CWindowWin32::FocusGained() {
    ClipCursorToWindow();
}

void CWindowWin32::FocusLost() {
    ClipCursor(NULL);
}

void CWindowWin32::ClipCursorToWindow() {
    RECT rect;
    GetClientRect(m_hWnd, &rect);
        
    POINT ul{};
    ul.x = rect.left;
    ul.y = rect.top;

    POINT lr{};
    lr.x = rect.right;
    lr.y = rect.bottom;

    MapWindowPoints(m_hWnd, nullptr, &ul, 1);
    MapWindowPoints(m_hWnd, nullptr, &lr, 1);

    rect.left = ul.x;
    rect.top = ul.y;

    rect.right = lr.x;
    rect.bottom = lr.y;

    ClipCursor(&rect);
}

void CWindowWin32::SetWindowMode(WindowMode new_mode, int resolution_x, int resolution_y) {
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

    SetWindowLongPtr(m_hWnd, GWL_STYLE, dwStyle);
    Logger::Msg(std::string("setting new mode: ").append(std::to_string(static_cast<int>(new_mode))).append(";").append(std::to_string(new_width).append("-").append(std::to_string(new_height))));
    SetWindowPos(m_hWnd, NULL, posX, posY, new_width, new_height, SWP_SHOWWINDOW | SWP_FRAMECHANGED);

    ClipCursorToWindow();
}
