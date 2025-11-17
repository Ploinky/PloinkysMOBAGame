#pragma once

#include <string>
#include <vector>
#include <map>
#include <optional>

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
    std::string iconId;
};

class CAnimationData {
public:
    std::string name;
    float fDuration;
};

class CModelData {
public:
    std::string id;
    std::map<std::string, CAnimationData> mapAnimations;
};

class CTransformData {
public:
    int nCollisionRadius;
};

class CNetworkData {
public:
    bool bSyncMovement;
};

class CMovementData {
public:
    int nSpeed;
};

class CHealthData {
public:
    int nMaxHealth;
};

class CNavigationData {

};

class CCharacterData {
public:
    std::string strId;
    std::string strName;
    std::map<int, std::string> mapAbilityIds;
    std::string modelId;
    std::optional<CTransformData> optTransformData;
    std::optional<CNetworkData> optNetworkData;
    std::optional<CMovementData> optMovementData;
    std::optional<CHealthData> optHealthData;
    std::optional<CNavigationData> optNavigationData;
};

class CGameData {
public:
    std::map<std::string, CCharacterData> mapCharacterData;
    std::map<std::string, CModelData> mapModelData;
    std::map<std::string, CAbilityData> mapAbilityData;
};