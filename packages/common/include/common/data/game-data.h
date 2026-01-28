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
    std::string strId;
} ImpactEffectFX_t;

typedef struct {
    EImpactEffectAffects eAffects;
    std::vector<float> vecDamage;
} ImpactEffectDamage_t;

typedef struct {
    EImpactEffectAffects eAffects;
    std::vector<float> vecHeal;
} ImpactEffectHeal_t;

typedef struct { 
    std::string strTemplateId;
} ImpactEffectSpawn_t;

typedef struct {
    std::vector<ImpactEffectDamage_t> vecDamageEffects;
    std::vector<ImpactEffectHeal_t> vecHealEffects;
    std::vector<ImpactEffectFX_t> vecFXEffects;
    std::vector<ImpactEffectSpawn_t> vecSpawnEffects;
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

class CIconData {
public:
    std::string id;
};

class CAudioAssetData {
public:
    std::string id;
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

class CIntentData {

};

class CTargetableData {
};

class CAudioData {
public:
    std::map<std::string, std::string> mapAudioIds;
};

class CEffectData {
public:
    std::string strId;
    std::string audioId;
};

class CPickupableData {
public:
};

class CUseableData {
public:
    std::string strAbilityId;
    int nUses = -1;
};

class CInventoryData {
public:
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
    std::optional<CIntentData> optIntentData;
    std::optional<CTargetableData> optTargetableData;
    std::optional<CAudioData> optAudioData;
    std::optional<CPickupableData> optPickupableData;
    std::optional<CUseableData> optUseableData;
    std::optional<CInventoryData> optInventoryData;
};

class CMapCameraVolume {
public:
    float fLeft;
    float fRight;
    float fBottom;
    float fTop;
    float fFront;
    float fBack;
};

class CMapData {
public:
    std::string strId;
    std::string navMeshDataFile;
    CMapCameraVolume camVolume;
};

class CGameData {
public:
    std::map<std::string, CCharacterData> mapCharacterData;
    std::map<std::string, CModelData> mapModelData;
    std::map<std::string, CAbilityData> mapAbilityData;
    std::map<std::string, CIconData> mapIconData;
    std::map<std::string, CAudioAssetData> mapAudioData;
    std::map<std::string, CEffectData> mapEffectData;
    std::map<std::string, CMapData> mapMapData;
};