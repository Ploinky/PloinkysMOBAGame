namespace P3D {
    class MouseInput {
    public:
        MouseInput();

        void SetMousePosition(short x, short y);
        short GetMouseX();
        short GetMouseY();
        bool IsButtonDown(int button);
        void SetButtonDown(int button);
        void SetButtonUp(int button);

    private:
        short x;
        short y;
        bool btnDown[3] { false, false, false };
    };
}