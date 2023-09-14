#include "new_packets.h"

namespace PMG::Networking {
    void StopCommandPacket::Read(std::vector<uint8_t>* data) {
        // huh?
    }

    void StopCommandPacket::Write(std::vector<uint8_t>* data) {
        packet_header_t header;
        header.type = PacketType::CMD_STOP;
        header.size = sizeof(packet_header_t);
        data->resize(header.size);
        std::memcpy(data->data(), &header, sizeof(packet_header_t));
    }
}