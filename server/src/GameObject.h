#pragma once

#include <Common/PMG_Common.h>
#include <NetworkManager.h>

class CGameState;
class CGameObject;

class IComponent {
public:
	virtual void Update(CGameState* pGameState, float fDelta) = 0;

	virtual void SetGameObject(CGameObject* pGameObject);

protected:
	CGameObject* m_pGameObject;
};

class CGameObject {
public:
	CGameObject();

	void Update(CGameState* pGameState, float fDelta);

	template<typename T>
	T* GetComponent() {
		for(IComponent* pComponent : m_vecComponents) {
			if(T* pSpecific = dynamic_cast<T*>(pComponent)) {
				return pSpecific;
			}
		}

		return nullptr;
	}


	void AddComponent(IComponent* pComponent);

	UnitId GetId();

private:
	UnitId m_idUnit;

	std::vector<IComponent*> m_vecComponents;
};