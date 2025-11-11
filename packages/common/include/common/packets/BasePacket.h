#ifndef _BASE_PACKET_H_
#define _BASE_PACKET_H_

#include "PacketType.h"
#include <vector>
#include <stdint.h>

typedef struct {
	PacketType type;
	size_t size;
} packet_header_t;

class BasePacket {
public:
	BasePacket(PacketType type) : type(type) {};
	virtual void Read(std::vector<uint8_t>* data) = 0;
	virtual void Write(std::vector<uint8_t>* data) = 0;

	PacketType type;
};

#endif