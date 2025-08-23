#include "Renderer.h"
#include "Gui.h"
#include "IClientState.h"
#include <vector>
#include "Settings.h"

class MainMenu : public IClientState {
public:
	MainMenu(IClientStateHandler* handler, int width, int height);
	~MainMenu();

	void Render(CRenderer* renderer) override;
	void Update(float dt) override;
	void MouseButtonPressed(int button) override;

private:
	GuiElement rootElement_;
	GuiButton buttonServerBrowser_;
	GuiButton buttonSettings_;
	GuiButton buttonBack_;
};
