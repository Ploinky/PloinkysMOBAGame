#pragma once

#include <string>
#include <map>

enum class EAbilityTargetType {
    UNIT
};

enum class EImpactEffectAffects {
    SELF,
    TARGET_UNIT,
};

typedef struct {
    EImpactEffectAffects eAffects;
    std::vector<float> vecDamage;
} ImpactEffectDamage_t;

typedef struct {
    std::vector<ImpactEffectDamage_t> vecDamageEffects;
} ImpactEffect_t;

class CAbilityData {
public:
    std::string strId;
    std::string strName;
    EAbilityTargetType eTargetType;
    float fCastRange;
    float fCastTime;
    float fCastPoint;
    float fCooldown;
    ImpactEffect_t effect;
};

class CCharacterData {
public:
    std::string strId;
    std::string strName;
    std::map<int, std::string> mapAbilityIds;
};

class CServerGameData {
public:
    std::map<std::string, CCharacterData> mapCharacterData;
    std::map<std::string, CAbilityData> mapAbilityData;
};