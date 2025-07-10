#include <Common/Packets/ScoreUpdatePacket.h>
#include <cstring>
#include <cstdlib>

void ScoreUpdatePacket::Read(std::vector<uint8_t>* data) {
	int offset = sizeof(packet_header_t);

	memcpy(&usTeam1Score, data->data() + offset, sizeof(usTeam1Score));
	offset += sizeof(usTeam1Score);

	memcpy(&usTeam2Score, data->data() + offset, sizeof(usTeam2Score));
}

void ScoreUpdatePacket::Write(std::vector<uint8_t>* data) {
	packet_header_t header{};
	header.type = type;
	header.size = sizeof(packet_header_t) + sizeof(usTeam1Score) + sizeof(usTeam2Score);

	size_t offset = data->size();
	data->resize(data->size() + header.size);

	memcpy(data->data() + offset, &header, sizeof(header));
	offset += sizeof(header);
	
	memcpy(data->data() + offset, &usTeam1Score, sizeof(usTeam1Score));
	offset += sizeof(usTeam1Score);
	
	memcpy(data->data() + offset, &usTeam2Score, sizeof(usTeam2Score));
}