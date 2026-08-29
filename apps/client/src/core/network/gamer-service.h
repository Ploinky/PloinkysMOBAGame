#pragma once

#include <memory>
#include <string>
#include <functional>

typedef struct {
    std::string name;
    std::string address;   // ip:port or connect string
    int playerCount;
    int maxPlayers;
    int ping;
} ServerInfo_t;

typedef struct {
    std::vector<uint8_t> vecData;
    int nWidth;
    int nHeight;
} Portrait_t;

using ServerListCallback = std::function<void(ServerInfo_t)>;

class IGamerService {
public:
    // Lifecycle
    static std::unique_ptr<IGamerService> Create();
    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;
    virtual void Update() = 0;

    // Player info
    virtual std::string GetPlayerName() = 0;
    virtual Portrait_t GetPlayerPortrait() = 0;

    // Server browser
    virtual void RequestLANServers(ServerListCallback callback) = 0;
    virtual void CancelQuery() = 0;
};