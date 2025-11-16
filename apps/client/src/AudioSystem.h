#pragma once

#include <map>
#include <string>
#include "client-asset-manager.h"
#include "core/audio/audio-engine.h"
#include "common/game/game-state.h"
#include "common/game/game-system.h"
#include "game/events/events.h"

class AudioSystem : public IGameSystem{
public:
	AudioSystem(IAudioEngine* pEngine, CClientAssetManager* pAssetManager);
	virtual void Update(CGameState* pGameState, float fDelta) override;

	void PlaySoundOnUnit(HSound hSound, UnitId idUnit);

	void SetListenerPosition(Vector3 vec3LisPos);
	void OnSpellHit(CGameState* pGameState, CSpellHitEvent* pHitEvent);
	void OnAttackStart(CGameState* pGameState, CAttackStartEvent* pHitEvent);

private:
	CClientAssetManager* m_pAssetManager;
	IAudioEngine* m_pEngine;
};
