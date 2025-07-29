#pragma once

#include <Common/PMG_Common.h>
#include <NetworkManager.h>

class CGameState;
class CGameObject;

class IComponent {
public:
	virtual void SetGameObject(CGameObject* pGameObject);

	// TODO private
	CGameObject* m_pGameObject;
protected:
};

class CGameObject {
public:
	CGameObject();

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