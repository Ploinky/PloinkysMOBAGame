#pragma once

#include <IClientState.h>
#include <Common/PMG_Common.h>
#include <memory>
#include <thread>
#include <string>
#include <vector>
#include <Renderer.h>

namespace PMG {
	class CLoadingState : public IClientState {
	public:
		CLoadingState(
			IClientStateHandler* pStateHandler,
			int iWidth,
			int iHeight
		);
		~CLoadingState();

		virtual void Update(float dt) override;
		virtual void Render(CRenderer* pRenderer) override;

	private:
		AssetManager* m_pAssetManager;
		CRenderer* m_pRenderer;
		int m_iLoadStep;
		bool m_bLoadingDone;
		std::vector<std::string> vecLoadText = {
			"Loading Assets...",
			"Loading Shaders..."
		};
		std::thread m_loadingThread;
	};
}