#include "Buff.h"
#include "Server.h"
#include <exception>

Buff::Buff() {
	m_iDuration = 0;
}

void Buff::Update(float dt) {
	m_iDuration += dt * 1000;
	
	throw std::runtime_error("we break this?");
	
}

void Buff::Apply() { // AttackableStats* stats, int* status_enable, int* status_disable) {
	throw std::runtime_error("we break this?");
}