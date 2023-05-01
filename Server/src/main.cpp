#include <iostream>
#include "server.h"
#include "logger.h"

int main(int argc, char** argv) {
    PMG::Logger::Msg("Starting Ploinky's MOBA Game Server...");

    PMG::Server* server = new PMG::Server();
    server->Start();

    return 0;
}