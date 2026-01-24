#pragma once

#include <map>
#include <string>
#include "client-asset-manager.h"
#include "core/audio/audio-engine.h"
#include "game/client-game-state.h"
#include "game/client-game-system.h"
#include "game/events/events.h"

class AudioSystem : public ISystem{
public:
	AudioSystem(IAudioEngine* pEngine, CClientAssetManager* pAssetManager);
	virtual void Update(CClientGameState* pGameState, float fDelta) override;

	void PlaySoundOnUnit(CClientGameState* pGameState, HSound hSound, UnitId idUnit);

	void SetListenerPosition(Vector3 vec3LisPos);
	void OnSpellHit(CClientGameState* pGameState, CSpellHitEvent* pHitEvent);
	void OnAttackStart(CClientGameState* pGameState, CAttackStartEvent* pHitEvent);
	void OnEntityDeath(CClientGameState* pGameState, CEntityDeathEvent* pDeathEvent);

private:
	CClientAssetManager* m_pAssetManager;
	IAudioEngine* m_pEngine;
};
