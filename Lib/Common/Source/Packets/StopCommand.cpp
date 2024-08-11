#include <Packets/StopCommand.h>
#include <cstring>

namespace PMG::Networking {
    void StopCommandPacket::Read(std::vector<uint8_t>* data) {
        // huh?
    }

    void StopCommandPacket::Write(std::vector<uint8_t>* data) {
        packet_header_t header;
        header.type = PacketType::CMD_STOP;
        header.size = sizeof(packet_header_t);

        size_t offset = data->size();
        data->resize(data->size() + header.size);
        memcpy(data->data() + offset, &header, sizeof(packet_header_t));
    }
}