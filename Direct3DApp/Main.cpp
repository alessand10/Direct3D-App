#include <Windows.h>
#include "D3DApp.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nShowCmd)
{
	D3DApp app;
	D3DApp::InitializationData appInitData{ hInstance, CreateSolidBrush(RGB(0, 0, 0))};
	app.initializeApp(&appInitData);
	MSG msg = {};
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			app.render();
		}
	}
	return S_OK;
}