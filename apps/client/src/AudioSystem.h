#pragma once

#include <map>
#include <string>
#include "client-asset-manager.h"
#include "core/audio/audio-engine.h"
#include "game/client-game-state.h"
#include "common/game/game-system.h"
#include "game/events/events.h"

class CClientGameState;

class AudioSystem : public IGameSystem<CClientGameState> {
public:
	AudioSystem(IAudioEngine* pEngine, CClientAssetManager* pAssetManager);
	virtual void Update(CClientGameState* pGameState, float fDelta) override;

	void PlaySoundOnUnit(CClientGameState* pGameState, HSound hSound, UnitId idUnit);

	void SetListenerPosition(Vector3 vec3LisPos);
	REGISTER_EVENT_HANDLER(AudioSystem, CEntityDeathEvent, OnEntityDeath)
	REGISTER_EVENT_HANDLER(AudioSystem, CSpellHitEvent, OnSpellHit)
	REGISTER_EVENT_HANDLER(AudioSystem, CAttackStartEvent, OnAttackStart)

private:
	CClientAssetManager* m_pAssetManager;
	IAudioEngine* m_pEngine;
};
