#include <core/input/input-manager.h>


CInputManager::CInputManager(Window* pWindow) {
    m_pWindow = pWindow;

    m_pWindow->e_mouseButtonPressed = [this](EMouseButton eBtn){this->OnMouseButtonPressed(eBtn);};
    m_pWindow->e_keyPressed = [this](EKeyCode eKeyCode){this->OnKeyPressed(eKeyCode);};
    m_pWindow->e_keyReleased = [this](EKeyCode eKeyCode){this->OnKeyReleased(eKeyCode);};

    // REGISTER KEYS??
    m_mapMenu.emplace(EKeyCode::ESCAPE, EInputAction::MENU_CLOSE);
    
    m_mapGame.emplace(EKeyCode::ESCAPE, EInputAction::GAME_ESCAPE);
    m_mapGame.emplace(EKeyCode::SPACE, EInputAction::GAME_FOCUS_UNIT);
    m_mapGame.emplace(EKeyCode::Q, EInputAction::GAME_CAST_SPELL_1);
    m_mapGame.emplace(EKeyCode::S, EInputAction::GAME_STOP);
}

void CInputManager::SetContext(EInputContext eInputContext) {
    m_eCurrentContext = eInputContext;
}

void CInputManager::OnKeyReleased(EKeyCode eKeyCode) {
    switch(m_eCurrentContext) {
        case EInputContext::MENU:
            if(m_mapMenu.contains(eKeyCode)) {
                on_action_released(m_mapMenu.at(eKeyCode));
            }
            break;
        case EInputContext::GAME:
            if(m_mapGame.contains(eKeyCode)) {
                on_action_released(m_mapGame.at(eKeyCode));
            }
        default:
            break;
    }
}

void CInputManager::OnMouseButtonPressed(EMouseButton eBtn) {
    if(m_eCurrentContext == EInputContext::MENU) {
        if(eBtn == EMouseButton::LEFT) {
            on_action(EInputAction::MENU_SELECT);
        }
    }

    if(m_eCurrentContext == EInputContext::GAME) {
        if(eBtn == EMouseButton::RIGHT) {
            on_action(EInputAction::GAME_SECONDARY);
        }
    }
}

void CInputManager::OnKeyPressed(EKeyCode eKeyCode) {
    switch(m_eCurrentContext) {
        case EInputContext::MENU:
            if(m_mapMenu.contains(eKeyCode)) {
                on_action(m_mapMenu.at(eKeyCode));
            }
            break;
        case EInputContext::GAME:
            if(m_mapGame.contains(eKeyCode)) {
                on_action(m_mapGame.at(eKeyCode));
            }
        default:
            break;
    }
}