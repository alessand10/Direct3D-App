#pragma once
#include <Windows.h>
#include "Common.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include "PixelShader.h"
#include "VertexShader.h"
#include <vector>
#include "Mesh.h"
#include "AppTypes.h"
#include "Camera.h"

using std::vector;

class D3DApp {
	HWND viewportWindow;
	WNDCLASSEX viewportClass;

	CBuffer constBufferPerFrame;

	/* App */
	Camera camera;
	vector<Mesh*> meshes = {};
	UINT nextFreeVBIndex = 0;
	UINT nextFreeIDBIndex = 0;
	float backgroundColour[4]{ 0.f, 0.f, 0.f, 1.f };

	/* Pipeline Objects */
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> deviceContext;
	D3D_FEATURE_LEVEL usedFeatureLevel;
	ComPtr<IDXGISwapChain> swapchain;
	ComPtr<ID3D11InputLayout> inputLayout;
	ID3D11PixelShader* bindedPixelShader;
	ID3D11VertexShader* bindedVertexShader;

	/* Textures */
	ComPtr<ID3D11Texture2D> depthStencilTex;

	/* Texture Resource Views */
	ComPtr<ID3D11RenderTargetView> renderTargetView;
	ComPtr<ID3D11DepthStencilView> depthStencilView;

	/* Buffers */
	ComPtr<ID3D11Buffer> vertexBuffer;
	ComPtr<ID3D11Buffer> indexBuffer;
	ComPtr<ID3D11Buffer> constantBuffer;
public:
	struct InitializationData;
	ID3D11Device* getDevice();
	void initializeApp(InitializationData* initData);
	void addMesh(Mesh* mesh);
	void render();

private:
	void setPipelineStateForRendering(Mesh* mesh);
	void createWindow(InitializationData* initData);
	HRESULT createDeviceAndContext();
	HRESULT createSwapChain(InitializationData* initData);
	HRESULT createRenderTargetView();
	HRESULT createDepthStencilView(InitializationData* initData);
	void setupOMState();
	void setupViewport(InitializationData* initData);
	HRESULT createAndBindVertexBuffer();
	HRESULT createAndBindIndexBuffer();
	HRESULT createAndBindInputLayout();
	HRESULT createAndBindConstantBuffer();
	void bindPixelShader(ID3D11PixelShader* pixelShader);
	void bindVertexShader(ID3D11VertexShader* vertexShader);
};

struct D3DApp::InitializationData {
	UINT resolution[2];
	UINT position[2];
	bool windowed;
	HINSTANCE hInstance;
	HBRUSH brush;

	InitializationData(HINSTANCE hInstance, HBRUSH brush, UINT resX = 1920U, UINT resY = 1080U, UINT posX = 0U, UINT posY = 0U, bool windowed = true) {
		resolution[0] = resX;
		resolution[1] = resY;
		position[0] = posX;
		position[1] = posY;
		this->brush = brush;
		this->hInstance = hInstance;
		this->windowed = windowed;
	}
};