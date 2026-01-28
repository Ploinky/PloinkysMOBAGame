#pragma once

#include "BasePacket.h"
#include "common/PMG_Common.h"

class CUseEntityPointCommand : public BasePacket {
public:
	CUseEntityPointCommand() : BasePacket(PacketType::CMD_USE_ENTITY_POINT) {};
	virtual void Read(std::vector<uint8_t>* data) override;
	virtual void Write(std::vector<uint8_t>* data) const override;
	
	UnitId idEntity;
    float x;
    float y;
};