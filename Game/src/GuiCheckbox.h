#include "GuiElement.h"
#include "Renderer.h"

namespace PMG {
	class GuiCheckbox : public GuiElement {
	public:
		void Render(Renderer* renderer);
		void MousePressed(int x, int y);

		bool IsSelected();
	private:
		bool isSelected_ = false;
	};
}