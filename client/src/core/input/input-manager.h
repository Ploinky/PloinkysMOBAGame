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
    std::function<void(EInputAction)> on_action_released = [](EInputAction){Logger::Err("Input release not handled!");};

private:
    Window* m_pWindow = nullptr;
    EInputContext m_eCurrentContext = EInputContext::MENU;
    std::map<EKeyCode, EInputAction> m_mapMenu;
    std::map<EKeyCode, EInputAction> m_mapGame;

    std::map<EInputAction, bool> m_mapAction;

    void OnMouseButtonPressed(EMouseButton eBtn);
    void OnKeyPressed(EKeyCode eBtn);
    void OnKeyReleased(EKeyCode eBtn);
};