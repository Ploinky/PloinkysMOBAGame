#pragma once

#include "IClientState.h"
#include "IClientStateHandler.h"
#include "Settings.h"
#include "Renderer.h"
#include "GuiButton.h"
#include "GuiSelector.h"

namespace PMG {
	class SettingsMenu : public IClientState {
	public:
		SettingsMenu(IClientStateHandler* handler, int width, int height, Settings* settings);
		~SettingsMenu();

		void Render(Renderer* renderer) override;
		void Update(float dt) override;
		void MouseButtonPressed(int button) override;

		void OnWindowResized() override;

	private:
		Settings* settings_;

		GuiButton buttonBack_;
		GuiSelector<WindowMode> guiSelector_ = GuiSelector<WindowMode>({
			WindowMode::WINDOWED, WindowMode::BORDERLESS, WindowMode::FULLSCREEN
			}, WindowMode::WINDOWED);

		bool videoModeExtended_ = false;
	};
}