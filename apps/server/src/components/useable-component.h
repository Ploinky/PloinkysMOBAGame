#pragma once


class CUseableComponent : public IComponent {
public:
    int nUses = -1;
    CAbilityData abilityData;
};