#ifndef _CAST_TARGET_COMMAND_H_
#define _CAST_TARGET_COMMAND_H_

#include "BasePacket.h"
#include <Common/pmg_types.h>
#include <stdint.h>

class CastTargetCommandPacket : public BasePacket {
public:
	CastTargetCommandPacket() : BasePacket(PacketType::CMD_CAST_TARGET) {};
	virtual void Read(std::vector<uint8_t>* data) override;
	virtual void Write(std::vector<uint8_t>* data) override;

	int spell_slot;
	UnitId target;
};

#endif