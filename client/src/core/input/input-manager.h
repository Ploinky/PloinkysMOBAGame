#pragma once

#include <core/input/input-action.h>
#include <core/input/key-code.h>
#include <core/window/Window.h>
#include <common/logger.h>

enum class EInputContext {
    MENU,
    GAME
};

class CInputManager {
public:
    CInputManager(Window* pWindow);
    void SetContext(EInputContext eNewContext);

    std::function<void(EInputAction)> on_action = [](EInputAction){Logger::Err("Input not handled!");};

private:
    Window* m_pWindow = nullptr;
    EInputContext m_eCurrentContext = EInputContext::MENU;
    std::map<EKeyCode, EInputAction> m_mapMenu;

    void OnMouseButtonPressed(EMouseButton eBtn);
    void OnKeyPressed(EKeyCode eBtn);
};