#include <packets/SpawnPacket.h>
#include <cstring>

void SpawnPacket::Read(std::vector<uint8_t>* data) {
	int offset = sizeof(packet_header_t);

	memcpy(&unit, data->data() + offset, sizeof(unit));
	offset += sizeof(unit);

	memcpy(&team, data->data() + offset, sizeof(team));
	offset += sizeof(team);

	memcpy(&x, data->data() + offset, sizeof(x));
	offset += sizeof(x);

	memcpy(&y, data->data() + offset, sizeof(y));
	offset += sizeof(y);

	memcpy(&z, data->data() + offset, sizeof(z));
	offset += sizeof(z);

	memcpy(&entTypeLen, data->data() + offset, sizeof(entTypeLen));
	offset += sizeof(entTypeLen);

	strEntId.resize(entTypeLen);
	memcpy(strEntId.data(), data->data() + offset, entTypeLen);
	offset += entTypeLen;
}

void SpawnPacket::Write(std::vector<uint8_t>* data) {
	entTypeLen = strEntId.length();

	packet_header_t header{};
	header.type = type;
	header.size = sizeof(packet_header_t) + sizeof(unit) + sizeof(team) + sizeof(x) + sizeof(y) + sizeof(z) + sizeof(entTypeLen) + entTypeLen;

	size_t offset = data->size();
	data->resize(data->size() + header.size);


	memcpy(data->data() + offset, &header, sizeof(header));
	offset += sizeof(header);
	
	memcpy(data->data() + offset, &unit, sizeof(unit));
	offset += sizeof(unit);
	
	memcpy(data->data() + offset, &team, sizeof(team));
	offset += sizeof(team);
	
	memcpy(data->data() + offset, &x, sizeof(x));
	offset += sizeof(x);
	
	memcpy(data->data() + offset, &y, sizeof(y));
	offset += sizeof(y);

	memcpy(data->data() + offset, &z, sizeof(z));
	offset += sizeof(z);
	
	memcpy(data->data() + offset, &entTypeLen, sizeof(entTypeLen));
	offset += sizeof(entTypeLen);

	memcpy(data->data() + offset, strEntId.data(), entTypeLen);
}