#pragma once

#include "IClientState.h"
#include "IClientStateHandler.h"
#include "Settings.h"
#include "Renderer.h"
#include "GuiButton.h"
#include "GuiSelector.h"

class SettingsMenu : public IClientState {
public:
	SettingsMenu(IClientStateHandler* handler, int width, int height, Settings* settings);
	~SettingsMenu();

	void Render(CRenderer* renderer) override;
	void Update(float dt) override;
	void MouseButtonPressed(int button) override;

	void OnWindowResized() override;

	virtual void Action(EInputAction eAction) override;

private:
	Settings* settings_;

	GuiElement rootElement_;
	GuiButton buttonBack_;
	GuiSelector<WindowMode> guiSelector_ = GuiSelector<WindowMode>({
		WindowMode::WINDOWED, WindowMode::BORDERLESS, WindowMode::FULLSCREEN
		}, WindowMode::WINDOWED);

	bool videoModeExtended_ = false;
};
