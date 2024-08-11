#include <LoadingState.h>

#include <Windows.h>

namespace PMG {
	CLoadingState::CLoadingState(IClientStateHandler* pStateHandler, int iWidth, int iHeight
	) : IClientState(pStateHandler, iWidth, iHeight) {
		this->m_pAssetManager = pStateHandler->GetAssetManager();
		this->m_pRenderer = pStateHandler->GetRenderer();

		m_bLoadingDone = false;
		m_iLoadStep = 0;
		m_loadingThread = std::thread([this]() {
			m_pAssetManager->LoadPakFile("Persons/ChessPerson.pak");
			m_pAssetManager->LoadPakFile("Buildings/Tower.pak");
			m_pAssetManager->LoadPakFile("Persons/Minion.pak");
			m_pAssetManager->LoadPakFile("UI/Buttons/MenuButton.pak");
			m_pAssetManager->LoadPakFile("UI/MoveTo.pak");
			m_pAssetManager->LoadPakFile("Maps/Map1.pak");
			m_pAssetManager->LoadPakFile("Shaders.pak");

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
			handler_->OpenMainMenu();
		}
	}

	void CLoadingState::Render(CRenderer* pRenderer) {
		pRenderer->RenderText(windowWidth_ / 2 - 50, windowHeight_ / 2 - 50, 100, 100, vecLoadText.at(m_iLoadStep));
	}
}