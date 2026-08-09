#include "gamer-service.h"

#include <steam/steam_api.h>

class CSteamGamerService : public IGamerService,  ISteamMatchmakingServerListResponse {
public:
    virtual bool Initialize() override;
    virtual void Shutdown() override;
    virtual void Update() override;

    virtual std::string GetPlayerName() override;
    
    void RequestLANServers(ServerListCallback callback) override;
    void CancelQuery() override;
    
	void ServerResponded(HServerListRequest hRequest, int iServer); 
	void ServerFailedToRespond( HServerListRequest hRequest, int iServer ); 
	void RefreshComplete(HServerListRequest hRequest, EMatchMakingServerResponse response); 

private:
    ServerListCallback m_callback;
};