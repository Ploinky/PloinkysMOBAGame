#pragma once


class CUseableComponent : public IComponent {
public:
    int nUses = -1;
    std::string strAbilityId;
};