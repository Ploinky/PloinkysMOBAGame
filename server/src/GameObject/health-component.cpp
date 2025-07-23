#include "health-component.h"

void CHealthComponent::TakeDamage(int nDamage) {
    m_nHealth -= nDamage;
}

int CHealthComponent::GetCurrentHealth() {
    return m_nHealth;
}

int CHealthComponent::GetMaxHealth() {
    return m_nMaxHealth;
}