#include <iostream>
#include <Server.h>
#include <Common/PMG_Common.h>

int main(int argc, char** argv) {
    Logger::Msg("Starting Ploinky's MOBA Game Server...");
	
    Server* server = new Server();
    server->Start();

    return 0;
}