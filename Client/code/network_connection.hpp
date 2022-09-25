#include "ws2tcpip.h"
#include "winsock2.h"
#include <string>
#include <list>

namespace PMG {
    class NetworkConnection {
    public:
        void Connect(std::string ip);
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