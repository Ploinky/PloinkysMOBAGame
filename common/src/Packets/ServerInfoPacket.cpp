#include <packets/ServerInfoPacket.h>
#include <cstring>
#include <cstdlib>

void ServerInfoPacket::Read(std::vector<uint8_t>* data) {
	int offset = sizeof(packet_header_t);

	memcpy(&ubPlayerCount, data->data() + offset, sizeof(ubPlayerCount));
	offset += sizeof(ubPlayerCount);

	memcpy(&ubPlayerMaxCount, data->data() + offset, sizeof(ubPlayerMaxCount));
	offset += sizeof(ubPlayerMaxCount);

	memcpy(&ubNameLen, data->data() + offset, sizeof(ubNameLen));
	offset += sizeof(ubNameLen);

	szName = (char*) std::malloc(ubNameLen);
	memcpy(szName, data->data() + offset, ubNameLen);
	offset += ubNameLen;
}

void ServerInfoPacket::Write(std::vector<uint8_t>* data) {
	packet_header_t header{};
	header.type = type;
	header.size = sizeof(packet_header_t) + sizeof(ubPlayerCount) + sizeof(ubPlayerMaxCount) + sizeof(ubNameLen) + ubNameLen;

	size_t offset = data->size();
	data->resize(data->size() + header.size);


	memcpy(data->data() + offset, &header, sizeof(header));
	offset += sizeof(header);
	
	memcpy(data->data() + offset, &ubPlayerCount, sizeof(ubPlayerCount));
	offset += sizeof(ubPlayerCount);
	
	memcpy(data->data() + offset, &ubPlayerMaxCount, sizeof(ubPlayerMaxCount));
	offset += sizeof(ubPlayerMaxCount);
	
	memcpy(data->data() + offset, &ubNameLen, sizeof(ubNameLen));
	offset += sizeof(ubNameLen);

	memcpy(data->data() + offset, szName, ubNameLen);
}