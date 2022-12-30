#pragma once
#include <Windows.h>
#include "Common.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include "PixelShader.h"
#include "VertexShader.h"
#include "ComputeShader.h"
#include <vector>
#include "Mesh.h"
#include "AppTypes.h"
#include "Camera.h"
#include "AppTimer.h"
#include <string>

using std::vector;
using std::string;
extern class D3DApp* appRef;

struct TextureView {
	ComPtr<ID3D11Resource> texture;
	ComPtr<ID3D11ShaderResourceView> resourceView;
};

class D3DApp {
	HWND viewportWindow;
	WNDCLASSEX viewportClass;

	CBuffer constBufferPerFrame;

	/* Camera */
	Camera camera;
	const XMFLOAT4 defaultCamLoc = { 10.f, 10.f, 10.f, 1.f };
	const XMFLOAT4 defaultCamViewTarget = { 0.f, 0.f, 0.f, 1.f };
	const float zoomMultiple = 1.2f;

	/* App */
	vector<Mesh*> meshes = {};
	UINT nextFreeVBIndex = 0;
	UINT nextFreeIDBIndex = 0;
	float backgroundColour[4]{ 0.f, 0.f, 0.f, 1.f };
	class AppTimer timer;
	vector<ComPtr<ID3D11Texture2D>> textures = {};
	vector<ComPtr<ID3D11ShaderResourceView>> srvs = {};
	vector<ComPtr<ID3D11UnorderedAccessView>> uavs = {};


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
	AppSettings settings;
	struct InitializationData;
	ID3D11Device* getDevice();
	void initializeApp(InitializationData* initData);
	ComPtr<ID3D11Texture2D> createTextureFromFile(string path);
	ComPtr<ID3D11Texture2D> createTexture(D3D11_TEXTURE2D_DESC* texDesc, D3D11_SUBRESOURCE_DATA* initialData = nullptr);
	ComPtr<ID3D11Texture2D> createTexture(UINT16 width, UINT16 height, DXGI_FORMAT fmt, D3D11_USAGE usage, UINT bindFlag, D3D11_SUBRESOURCE_DATA* initialData = nullptr);
	ComPtr<ID3D11UnorderedAccessView> createUAV(ComPtr<ID3D11Resource> resource);
	ComPtr<ID3D11ShaderResourceView> createSRV(ComPtr<ID3D11Resource> resource);

	void setComputeShader(ComPtr<ID3D11ComputeShader> cs);
	void setPixelShader(ComPtr<ID3D11PixelShader> ps);
	void setVertexShader(ComPtr<ID3D11VertexShader> vs);
	void beginRendering();
	void endRendering();
	void renderMesh(Mesh* mesh);

	void addMesh(Mesh* mesh);
	void moveMouse(int x, int y, WPARAM wParam);
	void scrollMouse(int scrollMultiple);
	void centerViewport();
	void bindSRV(ShaderTypeSRV type, ComPtr<ID3D11ShaderResourceView> srv, UINT slot);
	void bindUAV(ShaderTypeUAV type, ComPtr<ID3D11UnorderedAccessView> uav, UINT slot);
	void dispatchComputeShader(UINT threadGroupCountX, UINT threadGroupCountY, UINT threadGroupCountZ);
	void destroyObjects();

private:
	// Initialization
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

	// Runtime
	void rotateViewport(float horizontalAngle, float verticalAngle);
	void panViewport(float x, float y);

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