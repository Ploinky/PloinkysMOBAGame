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
				data.vec2CurrPos = move.vec2Pos;
				InvalidateRect(hWnd, nullptr, true);
			}
			if (wParam == VK_ESCAPE) {
				PostQuitMessage(0);
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
	data.pAgent->position = { 1000, -1000 };
	data.pAgent->nCollisionRadius = 50;
	data.pMap->m_pMesh->LoadFromFile("map1");
	data.pMap->m_pGrid = new NavigationCellGrid(data.pMap->m_pMesh);
	NavigationGridAgent* agt = data.pMap->CreateAgent();
	agt->position = { 1500, -1500 };
	agt->nCollisionRadius = 60;
	agt->UnitId = 0;
	NavigationGridAgent* agt1 = data.pMap->CreateAgent();
	agt1->position = { 1525, -1475 };
	agt1->nCollisionRadius = 50;
	agt1->UnitId = 1;
	NavigationGridAgent* agt2 = data.pMap->CreateAgent();
	agt2->position = { 1530, -1445 };
	agt2->nCollisionRadius = 40;
	agt2->UnitId = 2;
	NavigationGridAgent* agt3 = data.pMap->CreateAgent();
	agt3->position = { 2000, -900 };
	agt3->nCollisionRadius = 30;
	agt3->UnitId = 3;
	NavigationGridAgent* agt4 = data.pMap->CreateAgent();
	agt4->position = { 1000, -900 };
	agt4->nCollisionRadius = 200;
	agt4->UnitId = 4;


	std::vector<Vector2> path = data.pMap->GetPath(agt3, agt3->position, {2200, -900});
	agt3->path = data.pMap->GetPath(agt3, agt3->position, {2200, -900});

	NavigationGridAgent* agt5 = data.pMap->CreateAgent();
	agt5->position = { 2100, -900 };
	agt5->nCollisionRadius = 10;

	while(agt3->position != Vector2(2200, -900)) {
		StepResult_t stepResult = data.pMap->Step(agt3, agt3->position, 10);
		if (stepResult.bBlocked) {
			agt3->path = data.pMap->GetPath(agt3, agt3->position, {2200, -900});
		}

		agt3->position = stepResult.vec2Pos;
	}

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