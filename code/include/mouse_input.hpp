namespace P3D {
    class MouseInput {
    public:
        MouseInput();

        void SetMousePosition(short x, short y);
        short GetMouseX();
        short GetMouseY();

    private:
        short x;
        short y;
    };
}