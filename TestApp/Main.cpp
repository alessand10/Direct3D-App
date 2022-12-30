#include <Windows.h>
#include "D3DApp.h"
#include <iostream>


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nShowCmd)
{
	D3DApp app;
	D3DApp::InitializationData appInitData{ hInstance, CreateSolidBrush(RGB(0, 0, 0)) };
	app.initializeApp(&appInitData);
	app.settings.lockMaxFPS = false;

	app.bindSRV(ShaderTypeSRV::PS,
		app.createSRV(app.createTextureFromFile("C:/Users/Alessandro Genovese/Desktop/womanyellingcat-1573233850.jpg")),
		0U);

	app.bindSRV(ShaderTypeSRV::PS,
		app.createSRV(app.createTextureFromFile("C:/Users/Alessandro Genovese/Desktop/5a7801cc489349203e554aec5af3900e.jpg")),
		1U);

	app.bindSRV(ShaderTypeSRV::PS,
		app.createSRV(app.createTextureFromFile("C:/Users/Alessandro Genovese/Desktop/hilly_terrain_01_puresky_4k.jpg")),
		2U);

	T_TEX2D test = app.createTexture(800U, 800U, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_USAGE_DEFAULT, D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE);
	T_SRV testSRV = app.createSRV(test);
	T_UAV testUAV = app.createUAV(test);

	ComputeShader cs("../Debug/BasicCompute.cso", &app);
	VertexShader cubeVS("../Debug/VertexShader.cso", &app);
	PixelShader cubePS("../Debug/PixelShader.cso", &app);
	app.setComputeShader(cs.computeShader);
	app.setPixelShader(cubePS.pixelShader);
	app.setVertexShader(cubeVS.vertexShader);

	Mesh mesh;
	mesh.importFromOBJ("C:/Users/Alessandro Genovese/Desktop/SkySphere.obj");
	app.addMesh(&mesh);

	MSG msg = {};
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			app.beginRendering();
			app.bindUAV(ShaderTypeUAV::CS, testUAV, 0U);
			app.dispatchComputeShader(40u, 40u, 1u);
			app.bindUAV(ShaderTypeUAV::CS, nullptr, 0U);
			//app.bindSRV(ShaderTypeSRV::PS, testSRV, 3U);
			//app.renderMesh(&mesh);
			//app.bindSRV(ShaderTypeSRV::PS, nullptr, 3U);
			app.endRendering();
		}
	}
	return S_OK;
}
