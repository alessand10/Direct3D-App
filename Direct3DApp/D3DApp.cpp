#include "D3DApp.h"
#include <d3d11.h>
#include "AppTypes.h"

D3DApp::D3DApp()
{

}

void D3DApp::createWindow(InitializationData* initData) {
	viewportClass.lpszClassName = L"Viewport";
	viewportClass.hInstance = initData->hInstance;
	viewportClass.lpfnWndProc = DefWindowProc;
	viewportClass.hIcon = LoadIconW(initData->hInstance, NULL); //Don't care about Icon right now
	viewportClass.hCursor = LoadCursorW(initData->hInstance, IDC_HAND);
	viewportClass.cbSize = sizeof(WNDCLASSEX);
	viewportClass.style = CS_HREDRAW | CS_VREDRAW;
	viewportClass.lpszMenuName = NULL;
	viewportClass.cbClsExtra = 0;
	viewportClass.cbWndExtra = 0;
	viewportClass.hbrBackground = CreateSolidBrush(RGB(85, 85, 85));
	viewportClass.hIconSm = LoadIconW(initData->hInstance, NULL);
	RegisterClassExW(&viewportClass);

	viewportWindow = CreateWindowExW(0L,
		L"Viewport",
		L"Viewport", WS_VISIBLE | WS_OVERLAPPEDWINDOW,
		initData->position[0], initData->position[1],
		initData->resolution[0], initData->resolution[1],
		NULL, HMENU(0), initData->hInstance,
		NULL);
}

void D3DApp::initializeApp(InitializationData* initData)
{
	createWindow(initData);
	createDeviceAndContext();
	createSwapChain(initData);
	createRenderTargetView();
	createDepthStencilView(initData);
	setupOMState();
	setupViewport(initData);
	createAndBindVertexBuffer();
	createAndBindIndexBuffer();

	//pShader.initialize("C:/Users/Alessandro Genovese/source/repos/Direct3DApp/Debug/TestPixelShader.cso", device.Get());
	//deviceContext->PSSetShader(pShader.pixelShader.Get(), nullptr, 0);

}

HRESULT D3DApp::createDeviceAndContext()
{
	const int numFeatureLevels = 1;
	D3D_FEATURE_LEVEL supportedFeatureLevels[numFeatureLevels] = { D3D_FEATURE_LEVEL_11_1 };
	return D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, supportedFeatureLevels, numFeatureLevels, D3D11_SDK_VERSION, device.GetAddressOf(), &usedFeatureLevel, deviceContext.GetAddressOf());
}

HRESULT D3DApp::createSwapChain(InitializationData* initData)
{
	IDXGIDevice* dxgiDevice;
	device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);
	IDXGIAdapter* dxgiAdapter;
	dxgiDevice->GetAdapter(&dxgiAdapter);
	IDXGIFactory* dxgiFactory;
	dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	swapChainDesc.BufferDesc.Width = initData->resolution[0];
	swapChainDesc.BufferDesc.Height = initData->resolution[1];
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60U;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1U;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.Windowed = initData->windowed;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.Flags = 0U;
	swapChainDesc.OutputWindow = viewportWindow;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;


	return dxgiFactory->CreateSwapChain(dxgiDevice, &swapChainDesc, swapchain.GetAddressOf());
}

HRESULT D3DApp::createRenderTargetView() {
	ID3D11Texture2D* backbuffer = nullptr;
	swapchain->GetBuffer(0U, __uuidof(ID3D11Texture2D), (void**)&backbuffer);
	return device->CreateRenderTargetView(backbuffer, nullptr, renderTargetView.GetAddressOf());
}

HRESULT D3DApp::createDepthStencilView(InitializationData* initData) {
	D3D11_TEXTURE2D_DESC depthStencilTexDesc;
	depthStencilTexDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilTexDesc.ArraySize = 1U;
	depthStencilTexDesc.Height = initData->resolution[1];
	depthStencilTexDesc.Width = initData->resolution[0];
	depthStencilTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilTexDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilTexDesc.CPUAccessFlags = 0U;
	depthStencilTexDesc.MiscFlags = 0;
	depthStencilTexDesc.MipLevels = 1;
	depthStencilTexDesc.SampleDesc.Count = 1U;
	depthStencilTexDesc.SampleDesc.Quality = 0U;

	HRESULT result = device->CreateTexture2D(&depthStencilTexDesc, nullptr, depthStencilTex.GetAddressOf());
	if (FAILED(result)) return result;
	return device->CreateDepthStencilView(depthStencilTex.Get(), nullptr, depthStencilView.GetAddressOf());
}

void D3DApp::setupOMState() {

	deviceContext->OMSetRenderTargets(1U, renderTargetView.GetAddressOf(), depthStencilView.Get());
}

void D3DApp::setupViewport(InitializationData* initData)
{
	D3D11_VIEWPORT d3dViewport;
	d3dViewport.Width = initData->resolution[0];
	d3dViewport.Height = initData->resolution[1];
	d3dViewport.MaxDepth = 1.0f;
	d3dViewport.MinDepth = 0.0f;
	d3dViewport.TopLeftX = initData->position[0];
	d3dViewport.TopLeftY = initData->position[1];
	deviceContext->RSSetViewports(1, &d3dViewport);
}

HRESULT D3DApp::createAndBindVertexBuffer() {
	HRESULT result;
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * 1000;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(Vertex);
	UINT stridesAndOffsets = { 0 };

	if (FAILED(result = device->CreateBuffer(&vertexBufferDesc,
		0,
		vertexBuffer.GetAddressOf())))
		return result;

	deviceContext->IASetVertexBuffers(0U, 1U, vertexBuffer.GetAddressOf(), &stridesAndOffsets, &stridesAndOffsets);
	return result;
}

HRESULT D3DApp::createAndBindIndexBuffer()
{
	HRESULT result;
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.ByteWidth = sizeof(UINT) * 1000;
	indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	indexBufferDesc.ByteWidth = sizeof(UINT);

	if (FAILED(result = device->CreateBuffer(&indexBufferDesc,
		0,
		indexBuffer.GetAddressOf())))
		return result;

	deviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0U);
	return result;
}


/**
 * @brief Creates an input layout and binds it to the IA.
 * @note This input layout object can be reused with any vertex shader as long as the shader's input signature matches the input layout.
 */
HRESULT D3DApp::createAndBindInputLayout(void* vertexShaderBytecode, SIZE_T bytecodeLength) {
	HRESULT result;
	const int numDescs = 1;

	D3D11_INPUT_ELEMENT_DESC inputElementDescs[numDescs] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0U, 0U, D3D11_INPUT_PER_VERTEX_DATA, 0U}
	};

	if (FAILED(result = device->CreateInputLayout(inputElementDescs,
		numDescs,
		vertexShaderBytecode,
		bytecodeLength,
		inputLayout.GetAddressOf()))) 
		return result;

	deviceContext->IASetInputLayout(inputLayout.Get());
	return result;
}

/**
 * @brief Binds a pixel shader to the pipeline if not already bound.
 */
void D3DApp::bindPixelShader(ID3D11PixelShader* pixelShader) {
	if (pixelShader != bindedPixelShader) {
		deviceContext->PSSetShader(pixelShader, nullptr, 0);
		bindedPixelShader = pixelShader;
	}
}


/**
 * @brief Binds a vertex shader to the pipeline if not already bound.
 */
void D3DApp::bindVertexShader(ID3D11VertexShader* vertexShader) {
	if (vertexShader != bindedVertexShader) {
		deviceContext->VSSetShader(vertexShader, nullptr, 0);
		bindedVertexShader = vertexShader;
	}
}

void D3DApp::render() {
	deviceContext->ClearRenderTargetView(renderTargetView.Get(), backgroundColour);
	deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0U);

	

	swapchain->Present(0U, 0U);
}