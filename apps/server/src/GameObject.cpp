#include "GameObject.h"

static UnitId s_idNext = 0;

CGameObject::CGameObject() {
    m_idUnit = s_idNext++;
}

UnitId CGameObject::GetId() {
    return m_idUnit;
}