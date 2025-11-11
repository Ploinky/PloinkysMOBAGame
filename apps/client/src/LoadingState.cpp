#include <LoadingState.h>

CLoadingState::CLoadingState(IClientStateHandler* pStateHandler, int iWidth, int iHeight, std::string strServerAddress
) : IClientState(pStateHandler, iWidth, iHeight) {
	this->m_strServerAddress = strServerAddress;
	this->m_pAssetManager = pStateHandler->GetAssetManager();
	this->m_pRenderer = pStateHandler->GetRenderer();

	m_bLoadingDone = false;
	m_iLoadStep = 0;
	m_loadingThread = std::thread([this]() {

#ifndef _DEBUG
		m_pAssetManager->LoadPakFile("Persons/ChessPerson.pak");
		m_pAssetManager->LoadPakFile("Buildings/Tower.pak");
		m_pAssetManager->LoadPakFile("Persons/Minion.pak");
		m_pAssetManager->LoadPakFile("Persons/_Generic.pak");
		m_pAssetManager->LoadPakFile("UI/Buttons/MenuButton.pak");
		m_pAssetManager->LoadPakFile("UI/MoveTo.pak");
		m_pAssetManager->LoadPakFile("Maps/Map1.pak");
		m_pAssetManager->LoadPakFile("Shaders.pak");
		m_pAssetManager->LoadPakFile("Generic.pak");
		m_pAssetManager->LoadPakFile("characters/stormcaller.pak");
#endif
    
		m_pAssetManager->LoadGLBModel("map1", "data/Maps/map1/map1.glb");
		m_pAssetManager->LoadGLBModel("football_person", "data/Persons/chess_person/chess_person.glb");
		m_pAssetManager->LoadGLBModel("tower", "data/Buildings/Tower/tower.glb");
		m_pAssetManager->LoadGLBModel("missile", "data/Persons/chess_person/missile.glb");
		m_pAssetManager->LoadGLBModel("minion", "data/Persons/Minion/minion.glb");

		// m_pAssetManager->LoadCharacterManifest("stormcaller");
	
		m_iLoadStep++;

		m_pRenderer->LoadResources(m_pAssetManager);

		m_bLoadingDone = true;
	});
}

CLoadingState::~CLoadingState() {
}

void CLoadingState::Update(float dt) {
	if(m_bLoadingDone) {
		m_loadingThread.join();

		if(m_strServerAddress != "") {
			ClientNetworkManager* pServerNetworkManger = new ClientNetworkManager();
			NewState(new Lobby(m_strServerAddress, handler_, windowWidth_, windowHeight_));
		} else {
			handler_->OpenMainMenu();
		}
	}
}

void CLoadingState::Render(CRenderer* pRenderer) {
	pRenderer->RenderText(windowWidth_ / 2 - 50, windowHeight_ / 2 - 50, 100, 100, vecLoadText.at(m_iLoadStep));
}
