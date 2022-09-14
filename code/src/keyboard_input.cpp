#include "keyboard_input.hpp"

namespace P3D {
    void KeyboardInput::SetKeyDown(char key, bool down) {
        keys[(int)key] = down;
    }

    bool KeyboardInput::IsKeyDown(char key) {
        return keys[(int)key];
    }
}