#include "world-map.h"

#define SCALING_FACTOR 5

HBRUSH hBrushBlue = CreateSolidBrush(RGB(0, 0, 255));
HBRUSH hBrushRed = CreateSolidBrush(RGB(255, 0, 0));
HBRUSH hBrushLightGray = CreateSolidBrush(RGB(200, 200, 200));
HBRUSH hBrushGray = CreateSolidBrush(RGB(100,100, 100));
HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
HPEN hPenGreen = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
HBITMAP hBitMap;

LRESULT CALLBACK WorldMap_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;


	switch (msg) {
		case WM_CREATE: {
			CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
			AppData_t* pSharedData = (AppData_t*)cs->lpCreateParams;

			SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pSharedData);
			return 0;
		}
		case WM_ERASEBKGND:
			return 0;
		case WM_PAINT: {
			RECT rc;
			HDC hdcMem;
			HBITMAP hbmMem, hbmOld;
			HBRUSH hbrBkGnd;
			HFONT hfntOld;

			AppData_t* pSharedData = (AppData_t*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

			HDC hDc = BeginPaint(hWnd, &ps);


			GetClientRect(hWnd, &rc);
			hdcMem = CreateCompatibleDC(ps.hdc);
			hbmMem = CreateCompatibleBitmap(ps.hdc, rc.right - rc.left, rc.bottom - rc.top);
			hbmOld = (HBITMAP) SelectObject(hdcMem, hbmMem);

			hbrBkGnd = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
			FillRect(hdcMem, &rc, hbrBkGnd);
			DeleteObject(hbrBkGnd);

			// draw the mesh
			for (polygon_t* pol : pSharedData->pMap->m_pMesh->mesh) {
				int c = 0;
				for (Vector3 vert : pol->vertices) {
					if (c == 0) {
						POINT p;
						MoveToEx(hdcMem, vert.x /SCALING_FACTOR, -(vert.z /SCALING_FACTOR), &p);
					}
					else {
						LineTo(hdcMem, vert.x /SCALING_FACTOR, -(vert.z /SCALING_FACTOR));
					}
					c++;
				}
				LineTo(hdcMem, pol->vertices[0].x /SCALING_FACTOR, -(pol->vertices[0].z /SCALING_FACTOR));
			}

			NavigationCellGrid* pGrid = pSharedData->pMap->m_pGrid;
			// draw the grid
			for (int x = 0; x < pGrid->CellCountX; x++) {
				for (int y = 0; y < pSharedData->pMap->m_pGrid->CellCountY; y++) {
					POINT p;
					MoveToEx(hdcMem, (x * pGrid->CellWidth) /SCALING_FACTOR, -(y * pGrid->CellHeight /SCALING_FACTOR), &p);
					LineTo(hdcMem, (x * pGrid->CellWidth + pGrid->CellWidth)  /SCALING_FACTOR, -(y * pGrid->CellHeight /SCALING_FACTOR));
					LineTo(hdcMem, (x * pGrid->CellWidth + pGrid->CellWidth)  /SCALING_FACTOR, -((y * pGrid->CellHeight + pGrid->CellHeight) /SCALING_FACTOR));
					LineTo(hdcMem, (x * pGrid->CellWidth)  /SCALING_FACTOR, -((y * pGrid->CellHeight + pGrid->CellHeight) /SCALING_FACTOR));
					LineTo(hdcMem, (x * pGrid->CellWidth)  /SCALING_FACTOR, -((y * pGrid->CellHeight) /SCALING_FACTOR));
					if (!pSharedData->pMap->CanMoveTo(pSharedData->pAgent, pGrid->Cells[x + y * pGrid->CellCountX], false)) {
						RECT rect;
						rect.left = (x * pGrid->CellWidth) / SCALING_FACTOR;
						rect.right = rect.left + (pGrid->CellWidth / SCALING_FACTOR);
						rect.top = -(y * pGrid->CellHeight) / SCALING_FACTOR;
						rect.bottom = rect.top - (pGrid->CellHeight / SCALING_FACTOR);
						
						FillRect(hdcMem, &rect, (HBRUSH) GetStockObject(LTGRAY_BRUSH));
					}
				}
			}

			// draw blockers
			for (NavigationGridAgent* pBlocker : pSharedData->pMap->m_vecAgents) {
				SelectObject(hdcMem, GetStockObject(GRAY_BRUSH));
				float fHalfRadius = pBlocker->nCollisionRadius;
				Ellipse(hdcMem, (pBlocker->position.x - fHalfRadius) /SCALING_FACTOR, -(pBlocker->position.y - fHalfRadius) /SCALING_FACTOR, (pBlocker->position.x + fHalfRadius) /SCALING_FACTOR, -(pBlocker->position.y + fHalfRadius) /SCALING_FACTOR);
			}

			// draw the player
			RECT rect;
			rect.left = pSharedData->vec2CurrPos.x /SCALING_FACTOR - 2;
			rect.right = (pSharedData->vec2CurrPos.x /SCALING_FACTOR) + 2;
			rect.bottom = -pSharedData->vec2CurrPos.y /SCALING_FACTOR - 2;
			rect.top = (-pSharedData->vec2CurrPos.y /SCALING_FACTOR) + 2;
			FillRect(hdcMem, &rect, hBrushBlue);
			float fHalfRadius = pSharedData->pAgent->nCollisionRadius;
			SelectObject(hdcMem, hBrushBlue);
			Ellipse(hdcMem, (pSharedData->vec2CurrPos.x - fHalfRadius) /SCALING_FACTOR, -(pSharedData->vec2CurrPos.y - fHalfRadius) /SCALING_FACTOR, (pSharedData->vec2CurrPos.x + fHalfRadius) /SCALING_FACTOR, -(pSharedData->vec2CurrPos.y + fHalfRadius) /SCALING_FACTOR);

			// draw the coarse path
			if (pSharedData->vecVec2CoarsePath.size() > 0) {
				SelectObject(hdcMem, hPenGreen);
				POINT p;
				MoveToEx(hdcMem, pSharedData->vec2CurrPos.x /SCALING_FACTOR, -(pSharedData->vec2CurrPos.y /SCALING_FACTOR), &p);
				for (int i = 0; i < pSharedData->vecVec2CoarsePath.size(); i++) {
					Vector2 to = pSharedData->vecVec2CoarsePath.at(i);
					if (!LineTo(hdcMem, to.x /SCALING_FACTOR, -(to.y /SCALING_FACTOR))) {
						MessageBeep(MB_ICONERROR);
					}
				}
			}

			// draw the path
			if (pSharedData->pAgent->path.size() > 0) {
				SelectObject(hdcMem, hPen);
				POINT p;
				MoveToEx(hdcMem, pSharedData->vec2CurrPos.x /SCALING_FACTOR, -(pSharedData->vec2CurrPos.y /SCALING_FACTOR), &p);
				for (int i = 0; i < pSharedData->pAgent->path.size(); i++) {
					Vector2 to = pSharedData->pAgent->path.at(i);
					if (!LineTo(hdcMem, to.x /SCALING_FACTOR, -(to.y /SCALING_FACTOR))) {
						MessageBeep(MB_ICONERROR);
					}
				}
			}


			BitBlt(ps.hdc,
				rc.left, rc.top,
				rc.right - rc.left, rc.bottom - rc.top,
				hdcMem,
				0, 0,
				SRCCOPY);

			SelectObject(hdcMem, hbmOld);
			DeleteObject(hbmMem);
			DeleteDC(hdcMem);
			EndPaint(hWnd, &ps);
			return 0;
		}
		case WM_LBUTTONDOWN: {
			AppData_t* pSharedData = (AppData_t*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			pSharedData->vec2CurrPos.x = GET_X_LPARAM(lParam) *SCALING_FACTOR;
			pSharedData->vec2CurrPos.y = -(GET_Y_LPARAM(lParam) *SCALING_FACTOR);
			pSharedData->pAgent->position = pSharedData->vec2CurrPos;

			auto start = std::chrono::high_resolution_clock::now();
			pSharedData->pAgent->path = pSharedData->pMap->GetPath(pSharedData->pAgent,
				{ pSharedData->vec2CurrPos.x, pSharedData->vec2CurrPos.y },
				{ pSharedData->pAgent->target.x, pSharedData->pAgent->target.y }
			);
			auto end = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double, std::milli> elapsed = end - start;

			// Print to console (use OutputDebugString in Win32)
			std::string timeStr = "Execution time: " + std::to_string(elapsed.count()) + " ms\n";

			pSharedData->vecVec2CoarsePath = pSharedData->pMap->GetCoarseGridPath(pSharedData->pAgent,
				{ pSharedData->vec2CurrPos.x, pSharedData->vec2CurrPos.y },
				{ pSharedData->pAgent->target.x, pSharedData->pAgent->target.y });
			OutputDebugStringA(timeStr.c_str());

			InvalidateRect(hWnd, NULL, false);

			return 0;
		}
		case WM_RBUTTONDOWN: {
			AppData_t* pSharedData = (AppData_t*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			pSharedData->pAgent->target.x = GET_X_LPARAM(lParam) *SCALING_FACTOR;
			pSharedData->pAgent->target.y = -(GET_Y_LPARAM(lParam) *SCALING_FACTOR);

			auto start = std::chrono::high_resolution_clock::now();
			pSharedData->pAgent->path = pSharedData->pMap->GetPath(
				pSharedData->pAgent,
				{ pSharedData->vec2CurrPos.x, pSharedData->vec2CurrPos.y },
				{ pSharedData->pAgent->target.x, pSharedData->pAgent->target.y }
			);
			auto end = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double, std::milli> elapsed = end - start;

			// Print to console (use OutputDebugString in Win32)
			std::string timeStr = "Execution time: " + std::to_string(elapsed.count()) + " ms\n";

			pSharedData->vecVec2CoarsePath = pSharedData->pMap->GetCoarseGridPath(pSharedData->pAgent,
				{ pSharedData->vec2CurrPos.x, pSharedData->vec2CurrPos.y },
				{ pSharedData->pAgent->target.x, pSharedData->pAgent->target.y });
			OutputDebugStringA(timeStr.c_str());

			InvalidateRect(hWnd, NULL, false);
			return 0;
		}
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void RegisterWorldMap() {
	WNDCLASS wc{};
	wc.lpszClassName = WC_WORLD_MAP;
	wc.lpfnWndProc = WorldMap_WndProc;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);

	RegisterClass(&wc);
}