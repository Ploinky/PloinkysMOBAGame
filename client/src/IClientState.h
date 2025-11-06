#pragma once

#include "IClientStateHandler.h"
#include <stdint.h>
#include <core/input/input-action.h>

class CRenderer;

class IClientState {
public:
	virtual ~IClientState() {};
	virtual void Update(float dt) = 0;
	virtual void Render(CRenderer* renderer) = 0;

	virtual void Action(EInputAction eAction) {};
	virtual void ActionReleased(EInputAction eAction) {};
		
	// Input handling
	virtual void CharTyped(uint32_t ch) {};
	virtual void KeyPressed(uint32_t key) {};
	virtual void KeyReleased(uint32_t key) {};
	virtual void MouseButtonPressed(int button) {};
	virtual void MouseButtonReleased(int button) {};
	virtual void MouseMoved(int x, int y) {
		mouseX_ = x;
		mouseY_ = y;
	};
	void WindowResized(int newWidth, int newHeight) {
		windowWidth_ = newWidth;
		windowHeight_ = newHeight;

		OnWindowResized();
	}

	IClientStateHandler* handler_;

protected:
	virtual void OnWindowResized() {};

	IClientState(IClientStateHandler* handler, int width, int height) : handler_(handler) {
		mouseX_ = 0;
		mouseY_ = 0;
		windowWidth_ = width;
		windowHeight_ = height;
	};

	void NewState(IClientState* state) {
		handler_->NewState(state);
	}

	int mouseX_;
	int mouseY_;

	int windowWidth_;
	int windowHeight_;
};
