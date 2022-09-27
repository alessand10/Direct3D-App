#include <Windows.h>
#include "D3DApp.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nShowCmd)
{
	D3DApp app;
	D3DApp::InitializationData appInitData{ hInstance, CreateSolidBrush(RGB(0, 0, 0)) };
	app.initializeApp(&appInitData);

	VertexShader cubeVS("C:/Users/Alessandro Genovese/source/repos/Direct3DApp/Debug/VertexShader.cso", &app);
	PixelShader cubePS("C:/Users/Alessandro Genovese/source/repos/Direct3DApp/Debug/PixelShader.cso", &app);

	Mesh mesh;
	mesh.importFromOBJ("C:/Users/Alessandro Genovese/Desktop/cube.obj");
	mesh.setPixelShader(&cubePS);
	mesh.setVertexShader(&cubeVS);
	app.addMesh(&mesh);

	Mesh mesh2;
	mesh2.importFromOBJ("C:/Users/Alessandro Genovese/Desktop/cube2.obj");
	mesh2.setPixelShader(&cubePS);
	mesh2.setVertexShader(&cubeVS);
	app.addMesh(&mesh2);

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