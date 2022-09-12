#include "ws2tcpip.h"
#include "winsock2.h"
#include <string>
#include <list>

namespace P3D {
    class NetworkConnection {
    public:
        void Connect();
        void Close();

        void ReceiveMessages();
        void WriteMessage(std::string message);

        bool HasMessage();
        std::string NextMessage();
    private:
        SOCKET sock;
        std::list<std::string> messages;
    };
}