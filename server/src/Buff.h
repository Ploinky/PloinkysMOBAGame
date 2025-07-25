#pragma once

#include <Common/PMG_Common.h>

#include "GameObject.h"

class Buff {
public:
	Buff();

	void Update(float dt);
	virtual void Apply(); // AttackableStats* stats, int* status_enable, int* status_disable);

	bool should_remove = false;
private:
	int m_iDuration;
};
