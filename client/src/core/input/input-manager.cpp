#include <core/input/input-manager.h>


CInputManager::CInputManager(Window* pWindow) {
    m_pWindow = pWindow;

    m_pWindow->e_mouseButtonPressed = [this](EMouseButton eBtn){this->OnMouseButtonPressed(eBtn);};
    m_pWindow->e_keyPressed = [this](EKeyCode eKeyCode){this->OnKeyPressed(eKeyCode);};

    // REGISTER KEYS??
    m_mapMenu.emplace(EKeyCode::ESCAPE, EInputAction::MENU_CLOSE);
}

void CInputManager::SetContext(EInputContext eInputContext) {
    m_eCurrentContext = eInputContext;
}

void CInputManager::OnMouseButtonPressed(EMouseButton eBtn) {
    if(eBtn == EMouseButton::LEFT) {
        on_action(EInputAction::MENU_SELECT);
    }
}

void CInputManager::OnKeyPressed(EKeyCode eKeyCode) {
    switch(m_eCurrentContext) {
        case EInputContext::MENU:
            if(m_mapMenu.contains(eKeyCode)) {
                on_action(m_mapMenu.at(eKeyCode));
            }
            break;
        default:
            break;
    }
}