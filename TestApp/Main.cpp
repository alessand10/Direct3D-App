#include <Windows.h>
#include "D3DApp.h"
#include <iostream>


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nShowCmd)
{
	D3DApp app;
	D3DApp::InitializationData appInitData{ hInstance, CreateSolidBrush(RGB(0, 0, 0)) };
	app.initializeApp(&appInitData);

	VertexShader cubeVS("../Debug/VertexShader.cso", &app);
	PixelShader cubePS("../Debug/PixelShader.cso", &app);

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

	TCHAR NPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, NPath);
	std::cout << NPath;


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