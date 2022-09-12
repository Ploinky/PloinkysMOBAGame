#pragma once

#include "Windows.h"

namespace P3D {
    class KeyboardInput {
        public:
            bool IsKeyDown(char key);
            void SetKeyDown(char key, bool down);

        private:
            bool keys[0xFF]; 
    };
}