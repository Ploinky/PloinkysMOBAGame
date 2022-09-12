#include "mouse_input.hpp"

namespace P3D {
    MouseInput::MouseInput() {
        x = 0;
        y = 0;
    }

    void MouseInput::SetMousePosition(short newX, short newY) {
        x = newX;
        y = newY;
    }

    short MouseInput::GetMouseX() {
        return x;
    }

    short MouseInput::GetMouseY() {
        return y;
    }
}