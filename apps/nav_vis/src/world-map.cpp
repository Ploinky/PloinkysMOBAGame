#include "world-map.h"

#define SCALING_FACTOR 5

HBRUSH hBrushBlue = CreateSolidBrush(RGB(0, 0, 255));
HBRUSH hBrushRed = CreateSolidBrush(RGB(255, 0, 0));
HBRUSH hBrushLightGray = CreateSolidBrush(RGB(200, 200, 200));
HBRUSH hBrushGray = CreateSolidBrush(RGB(100,100, 100));
HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
HPEN hPenGreen = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
HPEN hPenBlue = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
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


			// draw blockers
			for (auto& [hBlocker, blocker] : pSharedData->pMap->m_blockers) {
				SelectObject( hdcMem, GetStockObject( NULL_BRUSH ) );
				// SelectObject(hdcMem, GetStockObject(GRAY_BRUSH));
				float fHalfRadius = blocker.radius + 50;
				Ellipse(hdcMem, (blocker.x - fHalfRadius) /SCALING_FACTOR, -(blocker.y - fHalfRadius) /SCALING_FACTOR, (blocker.x + fHalfRadius) /SCALING_FACTOR, -(blocker.y + fHalfRadius) /SCALING_FACTOR);
			}

			// draw the player
			RECT rect;
			rect.left = pSharedData->vec2CurrPos.x /SCALING_FACTOR - 2;
			rect.right = (pSharedData->vec2CurrPos.x /SCALING_FACTOR) + 2;
			rect.bottom = -pSharedData->vec2CurrPos.y /SCALING_FACTOR - 2;
			rect.top = (-pSharedData->vec2CurrPos.y /SCALING_FACTOR) + 2;
			FillRect(hdcMem, &rect, hBrushBlue);
			float fHalfRadius = 50;
			SelectObject(hdcMem, hBrushBlue);
			Ellipse(hdcMem, (pSharedData->vec2CurrPos.x - fHalfRadius) /SCALING_FACTOR, -(pSharedData->vec2CurrPos.y - fHalfRadius) /SCALING_FACTOR, (pSharedData->vec2CurrPos.x + fHalfRadius) /SCALING_FACTOR, -(pSharedData->vec2CurrPos.y + fHalfRadius) /SCALING_FACTOR);

			// draw the nodes
			if (!pSharedData->debugPath.vecNodes.empty()) {
				for (const auto& node : pSharedData->debugPath.vecNodes) {
					Ellipse(hdcMem, (node.vec2Pos.x - 10) /SCALING_FACTOR, -(node.vec2Pos.y - 10) /SCALING_FACTOR, (node.vec2Pos.x + 10) /SCALING_FACTOR, -(node.vec2Pos.y + 10) /SCALING_FACTOR);
				}
			}
			SelectObject(hdcMem, hPenGreen);
			SetArcDirection(hdcMem, AD_CLOCKWISE);

			// draw the edges
			if (!pSharedData->debugPath.vecEdges.empty()) {
				SelectObject(hdcMem, hPenGreen);
				POINT p;
				for (const auto& edge : pSharedData->debugPath.vecEdges) {
					Node_t start = pSharedData->debugPath.vecNodes[edge.start];
					Node_t end = pSharedData->debugPath.vecNodes[edge.end];
					if (edge.eType == EEdgeType::ARC) {
						RECT rc {
							edge.arcCenter.x/SCALING_FACTOR - edge.arcRadius/SCALING_FACTOR,
							-edge.arcCenter.y/SCALING_FACTOR - edge.arcRadius/SCALING_FACTOR,
							edge.arcCenter.x/SCALING_FACTOR + edge.arcRadius/SCALING_FACTOR,
							-edge.arcCenter.y/SCALING_FACTOR + edge.arcRadius/SCALING_FACTOR,
						};
							Arc(hdcMem,
							rc.left, rc.top, rc.right, rc.bottom,
							start.vec2Pos.x/SCALING_FACTOR, -start.vec2Pos.y/SCALING_FACTOR,
							end.vec2Pos.x/SCALING_FACTOR, -end.vec2Pos.y/SCALING_FACTOR
						);
					} else {
						MoveToEx(hdcMem, start.vec2Pos.x /SCALING_FACTOR, -(start.vec2Pos.y /SCALING_FACTOR), &p);
						LineTo(hdcMem, end.vec2Pos.x /SCALING_FACTOR, -(end.vec2Pos.y /SCALING_FACTOR));
					}
				}
			}

		
			// draw the long path
			if (pSharedData->vecVec2LongPath.size() > 0) {
				SelectObject(hdcMem, hPenBlue);
				POINT p;
				MoveToEx(hdcMem, pSharedData->vec2CurrPos.x /SCALING_FACTOR, -(pSharedData->vec2CurrPos.y /SCALING_FACTOR), &p);
				for (int i = 0; i < pSharedData->vecVec2LongPath.size(); i++) {
					Vector2 to = pSharedData->vecVec2LongPath.at(i);
					if (!LineTo(hdcMem, to.x /SCALING_FACTOR, -(to.y /SCALING_FACTOR))) {
						MessageBeep(MB_ICONERROR);
					}
				}
			}
			if (pSharedData->vecVec2ShortPath.size() > 0) {
				SelectObject(hdcMem, hPenBlue);
				POINT p;
				MoveToEx(hdcMem, pSharedData->vec2CurrPos.x /SCALING_FACTOR, -(pSharedData->vec2CurrPos.y /SCALING_FACTOR), &p);
				for (int i = 0; i < pSharedData->vecVec2ShortPath.size(); i++) {
					Vector2 to = pSharedData->vecVec2ShortPath.at(i);
					if (!LineTo(hdcMem, to.x /SCALING_FACTOR, -(to.y /SCALING_FACTOR))) {
						MessageBeep(MB_ICONERROR);
					}
				}
			}


			// draw the path
			if (pSharedData->path.size() > 0) {
				SelectObject(hdcMem, hPen);
				POINT p;
				MoveToEx(hdcMem, pSharedData->vec2CurrPos.x /SCALING_FACTOR, -(pSharedData->vec2CurrPos.y /SCALING_FACTOR), &p);
				for (int i = 0; i < pSharedData->path.size(); i++) {
					Vector2 to = pSharedData->path.at(i);
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

			auto start = std::chrono::high_resolution_clock::now();
			DebugPath_t path = pSharedData->pMap->GetDebugPath({ pSharedData->vec2CurrPos.x, pSharedData->vec2CurrPos.y },
				pSharedData->vec2Target
			);
			pSharedData->path = path.vecVec2FinalPath;
			pSharedData->vecVec2LongPath = path.vecVec2LongPath;
			pSharedData->vecVec2ShortPath = path.vecVec2ShortPath;
			pSharedData->debugPath = path;

			auto end = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double, std::milli> elapsed = end - start;

			// Print to console (use OutputDebugString in Win32)
			std::string timeStr = "Execution time: " + std::to_string(elapsed.count()) + " ms\n";

			OutputDebugStringA(timeStr.c_str());

			InvalidateRect(hWnd, NULL, false);

			return 0;
		}
		case WM_RBUTTONDOWN: {
			AppData_t* pSharedData = (AppData_t*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			pSharedData->vec2Target.x = GET_X_LPARAM(lParam) *SCALING_FACTOR;
			pSharedData->vec2Target.y = -(GET_Y_LPARAM(lParam) *SCALING_FACTOR);

			auto start = std::chrono::high_resolution_clock::now();
			DebugPath_t path = pSharedData->pMap->GetDebugPath({ pSharedData->vec2CurrPos.x, pSharedData->vec2CurrPos.y },
				{ pSharedData->vec2Target.x, pSharedData->vec2Target.y }
			);
			pSharedData->path = path.vecVec2FinalPath;
			pSharedData->vecVec2LongPath = path.vecVec2LongPath;
			pSharedData->vecVec2ShortPath = path.vecVec2ShortPath;
			pSharedData->debugPath = path;
			auto end = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double, std::milli> elapsed = end - start;

			// Print to console (use OutputDebugString in Win32)
			std::string timeStr = "Execution time: " + std::to_string(elapsed.count()) + " ms\n";

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