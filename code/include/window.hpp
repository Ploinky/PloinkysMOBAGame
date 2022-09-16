#pragma once

#include <Windows.h>
#include <functional>
#include <vector>

namespace P3D {
    // Forward declarations to avoid too many headers
    class Direct3D;

    class Window {
        public:
            std::function<void()> windowResizedHandler;

            int width;
            int height;

            Window();
            ~Window();

            HWND GetWindowHandle();
            
            // Show window on screen
            void Show();
            // Handle window events once per frame if applicable.
            // Depending on platform implementation, this could trigger window resized, key presses, mouse movement and much more...
            void HandleEvents();
            // Will return true whenever the window wants to close or is already closed
            bool ShouldClose();
            // Set value of 'shouldClose' to true. Can not be reversed -> window WILL close and P3D WILL quit!
            void SetShouldClose();

            void FocusGained();
            void FocusLost();

            void ClipCursorToWindow();

            void Resized(int width, int height);

            short GetMouseX();
            short GetMouseY();
            bool IsButtonDown(int button);
            bool IsKeyDown(char key);
            LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
            
        private:
            HWND windowHandle;
            bool shouldClose;

            // Keyboard input
            bool m_keys[0xFF]{ 0 };

            // Mouse input
            short m_mouseX;
            short m_mouseY;
            bool m_mouseBtnDown[3]{ false, false, false };
    };
}