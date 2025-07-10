#include "Buff.h"
#include "Server.h"

Buff::Buff() {
	m_iDuration = 0;
}

void Buff::Update(float dt) {
	m_iDuration += dt * 1000;
	
	throw std::exception("we break this?");
	
}

void Buff::Apply() { // AttackableStats* stats, int* status_enable, int* status_disable) {
	throw std::exception("we break this?");
}