#include "GuiElement.h"
#include "Renderer.h"

class GuiCheckbox : public GuiElement {
public:
	void Render(CRenderer* renderer);
	void MousePressed(int x, int y);

	bool IsSelected();
private:
	bool isSelected_ = false;
};
