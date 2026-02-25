#include <Windows.h>
#include <windowsx.h>

#include "world-map.h"
#include "app-data.h"

AppData_t data;

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_KEYDOWN: {
 			if (wParam == VK_SPACE) {
				auto move = data.pMap->Step(data.pAgent, data.vec2CurrPos, 100);
				data.vec2CurrPos = move;
				InvalidateRect(hWnd, nullptr, true);
			}
			return 0;
		}
		case WM_CLOSE: {
			PostQuitMessage(0);
			return 0;
		}
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
int  WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
	// Register custom components
	RegisterWorldMap();

	// Create app data
	data.pMap = new NavigationMap();
	data.pMap->m_pMesh = new NavMesh();
	data.pAgent = new NavigationGridAgent();
	data.pAgent->position = { 1000, 0, -1000 };
	data.pMap->m_pMesh->LoadFromFile("map1");
	NavigationGridAgent* agt = data.pMap->CreateAgent();
	agt->position = { 1500, 0, -1500 };
	agt->UnitId = 0;
	NavigationGridAgent* agt1 = data.pMap->CreateAgent();
	agt1->position = { 1800, 0, -1400 };
	agt1->UnitId = 1;
	NavigationGridAgent* agt2 = data.pMap->CreateAgent();
	agt2->position = { 1200, 0, -1400 };
	agt2->UnitId = 2;
	NavigationGridAgent* agt3 = data.pMap->CreateAgent();
	agt3->position = { 2000, 0, -900 };
	agt3->UnitId = 3;
	NavigationGridAgent* agt4 = data.pMap->CreateAgent();
	agt4->position = { 1000, 0, -900 };
	agt4->UnitId = 4;
	/*
	*/

	WNDCLASS wc{};
	wc.lpszClassName = TEXT("Window");
	wc.lpfnWndProc = WndProc;
	wc.style = CS_VREDRAW | CS_HREDRAW;
	wc.hbrBackground = HBRUSH(COLOR_WINDOW);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);

	RegisterClass(&wc);

	HWND hWndMainWindow = CreateWindow(
		TEXT("Window"),
		TEXT("NavigationVisualizer"),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		1024,
		860,
		NULL,
		NULL,
		GetModuleHandle(NULL),
		0
	);

	CreateWindow(
		WC_WORLD_MAP,
		TEXT(""),
		WS_VISIBLE | WS_CHILD,
		0,
		0,
		1024,
		860,
		hWndMainWindow,
		NULL,
		NULL,
		&data
	);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return EXIT_SUCCESS;
}