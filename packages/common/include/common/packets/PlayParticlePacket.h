#pragma once

#include "BasePacket.h"
#include <string>

class PlayParticlePacket : public BasePacket {
public:
	PlayParticlePacket() : BasePacket(PacketType::PCK_PLAY_PARTICLE) {};
	virtual void Read(std::vector<uint8_t>* data) override;
	virtual void Write(std::vector<uint8_t>* data) const override;

	unsigned int unit;
	float x;
	float y;
	std::string particle;
};