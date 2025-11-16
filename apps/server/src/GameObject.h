#pragma once

#include <common/PMG_Common.h>
#include "NetworkManager.h"

class CGameObject;

class IComponent {
public:
	UnitId idUnit;
};

class CGameObject {
public:
	CGameObject();

	UnitId GetId();

private:
	UnitId m_idUnit;
};