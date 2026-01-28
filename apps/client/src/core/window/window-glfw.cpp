#include "window-glfw.h"
#include <iostream>
#include <string>
#include <common/PMG_Common.h>
#include "../Resources/resource.h"
#include "Settings.h"
#include <map>

static const std::map<uint64_t, EKeyCode> g_mapKeyCode = {
    {GLFW_KEY_ESCAPE, EKeyCode::ESCAPE},
    {GLFW_KEY_SPACE, EKeyCode::SPACE},
    {GLFW_KEY_A, EKeyCode::A},
    {GLFW_KEY_B, EKeyCode::B},
    {GLFW_KEY_C, EKeyCode::C},
    {GLFW_KEY_D, EKeyCode::D},
    {GLFW_KEY_E, EKeyCode::E},
    {GLFW_KEY_F, EKeyCode::F},
    {GLFW_KEY_G, EKeyCode::G},
    {GLFW_KEY_H, EKeyCode::H},
    {GLFW_KEY_I, EKeyCode::I},
    {GLFW_KEY_J, EKeyCode::J},
    {GLFW_KEY_K, EKeyCode::K},
    {GLFW_KEY_L, EKeyCode::L},
    {GLFW_KEY_M, EKeyCode::M},
    {GLFW_KEY_N, EKeyCode::N},
    {GLFW_KEY_O, EKeyCode::O},
    {GLFW_KEY_P, EKeyCode::P},
    {GLFW_KEY_Q, EKeyCode::Q},
    {GLFW_KEY_R, EKeyCode::R},
    {GLFW_KEY_S, EKeyCode::S},
    {GLFW_KEY_T, EKeyCode::T},
    {GLFW_KEY_U, EKeyCode::U},
    {GLFW_KEY_V, EKeyCode::V},
    {GLFW_KEY_W, EKeyCode::W},
    {GLFW_KEY_X, EKeyCode::X},
    {GLFW_KEY_Y, EKeyCode::Y},
    {GLFW_KEY_Z, EKeyCode::Z},
    {GLFW_KEY_LEFT, EKeyCode::LEFT_ARROW},
    {GLFW_KEY_RIGHT, EKeyCode::RIGHT_ARROW},
    {GLFW_KEY_UP, EKeyCode::UP_ARROW},
    {GLFW_KEY_DOWN, EKeyCode::DOWN_ARROW},
    {GLFW_KEY_1, EKeyCode::NUM1}
};


Window* Window::Create(int res_x, int res_y, WindowMode mode) {
    return new CWindowGLFW(res_x, res_y, mode);
}

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
        if(nButton == GLFW_MOUSE_BUTTON_1) {
            self->e_mouseButtonReleased(EMouseButton::LEFT);
        }
        if(nButton == GLFW_MOUSE_BUTTON_2) {
            self->e_mouseButtonReleased(EMouseButton::RIGHT);
        }
    } 
} 

void GLFWMouseMoveCallback(GLFWwindow* pWindow, double dXPos, double dYPos) {
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(pWindow));
    self->e_mouseMoved(dXPos, dYPos);
}

void GLFWWindowCloseCallback(GLFWwindow* pWindow) {
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(pWindow));
    self->SetShouldClose();
}

void GLFWWindowFocusCallback(GLFWwindow* pWindow, int nFocused) {
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(pWindow));

    if(nFocused) {
        self->FocusGained();
    } else {
        self->FocusLost();
    }
}

CWindowGLFW::CWindowGLFW(int res_x, int res_y, WindowMode mode) {
    Logger::Msg("Creating window...");

    if(!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    this->width_ = res_x;
    this->height_ = res_y;

    shouldClose = false;

    // ------ Create window -----
    /*
    int posX, posY;

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    posX = (screenWidth - width_) / 2;
    posY = (screenHeight - height_) / 2;

    switch (mode) {
        case WindowMode::WINDOWED: {
            break;
        }
        case WindowMode::BORDERLESS:{
            width_ = screenWidth;
            height_ = screenHeight;
            posX = 0;
            posY = 0;
            break;
        }
        case WindowMode::FULLSCREEN:
        default: {
            width_ = screenWidth;
            height_ = screenHeight;
            posX = 0;
            posY = 0;
            break;
        }
    }

    */

    std::string window_title = "Ploinky's MOBA Game";

#ifdef _DEBUG
    window_title.append("__DEBUG BUILD__");
#endif
    
    m_pWindow = glfwCreateWindow(width_, height_, window_title.c_str(), NULL, NULL);

    if (m_pWindow == 0) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwSetWindowUserPointer(m_pWindow, this);
    glfwSetKeyCallback(m_pWindow, GLFWKeyCallback);
    glfwSetMouseButtonCallback(m_pWindow, GLFWMouseButtonCallback);
    glfwSetCursorPosCallback(m_pWindow, GLFWMouseMoveCallback);
    glfwSetWindowFocusCallback(m_pWindow, GLFWWindowFocusCallback);
    glfwSetWindowCloseCallback(m_pWindow, GLFWWindowCloseCallback);

    glfwMakeContextCurrent(m_pWindow);

    glClear(GL_COLOR_BUFFER_BIT);
}

CWindowGLFW::~CWindowGLFW() {
    glfwTerminate();
}

void CWindowGLFW::Show() {
    Logger::Msg("Showing window....");
    glfwShowWindow(m_pWindow);
    // ShowWindow(windowHandle, SW_SHOW);
}

void CWindowGLFW::HandleEvents() {
    glfwPollEvents();
}

HWindow CWindowGLFW::GetWindowHandle() {
    return m_pWindow;
}

void CWindowGLFW::Resized(int width_, int height_) {
    this->width_ = width_;
    this->height_ = height_;

    if(windowResizedHandler != nullptr) {
        windowResizedHandler();
    }
}

void CWindowGLFW::FocusGained() {
    ClipCursorToWindow();
}

void CWindowGLFW::FocusLost() {
    glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void CWindowGLFW::ClipCursorToWindow() {
    glfwSetInputMode(m_pWindow, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
    /*
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
    */

}

void CWindowGLFW::SetWindowMode(WindowMode new_mode, int resolution_x, int resolution_y) {
    /*
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
    */

}
