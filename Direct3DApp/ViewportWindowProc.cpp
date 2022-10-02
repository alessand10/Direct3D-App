#include "ViewportWindowProc.h"
#include "D3DApp.h"

#define C_KEY 0x43

LRESULT __stdcall ViewportWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	switch (message) {
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		if (wParam == C_KEY) appRef->centerViewport();
		break;

	case WM_MOUSEMOVE:
		appRef->moveMouse(LOWORD(lParam), HIWORD(lParam), wParam);
		break;

	case WM_MOUSEWHEEL:
		appRef->scrollMouse(GET_WHEEL_DELTA_WPARAM(wParam));
		break;

	default:
		return DefWindowProcW(hWnd, message, wParam, lParam);
	}

	return 0;
}