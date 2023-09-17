#include "new_packets.h"

namespace PMG::Networking {
    void UnitMovePacket::Read(std::vector<uint8_t>* data) {
        int offset = sizeof(packet_header_t);

        std::memcpy(&unit, data->data() + offset, sizeof(unit));
        offset += sizeof(unit);

        std::memcpy(&x, data->data() + offset, sizeof(x));
        offset += sizeof(x);
        
        std::memcpy(&y, data->data() + offset, sizeof(y));
        offset += sizeof(y);
        
        std::memcpy(&z, data->data() + offset, sizeof(z));
        offset += sizeof(z);
        
        std::memcpy(&r, data->data() + offset, sizeof(r));
        offset += sizeof(r);

    }

    void UnitMovePacket::Write(std::vector<uint8_t>* data) {
        packet_header_t header{};
        header.type = type;
        header.size = sizeof(packet_header_t) + sizeof(unit) + sizeof(x) + sizeof(y) + sizeof(z) + sizeof(r);

        data->resize(header.size);

        int offset = 0;
        std::memcpy(data->data() + offset, &header, sizeof(header));
        offset += sizeof(header);

        std::memcpy(data->data() + offset, &unit, sizeof(unit));
        offset += sizeof(unit);
        
        std::memcpy(data->data() + offset, &x, sizeof(x));
        offset += sizeof(x);
        
        std::memcpy(data->data() + offset, &y, sizeof(y));
        offset += sizeof(y);
        
        std::memcpy(data->data() + offset, &z, sizeof(z));
        offset += sizeof(z);
        
        std::memcpy(data->data() + offset, &r, sizeof(r));
    };
}