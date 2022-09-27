#include "D3DApp.h"
#include <d3d11.h>
#include "AppTypes.h"
#include "FileReader.h"

D3DApp::D3DApp()
{

}

ID3D11Device* D3DApp::getDevice() {
	return device.Get();
}

/**
 * @brief Registers a simple window class for the viewport and creates the window.
 * @param initData The data passed to the app for initialization purposes.
 */
void D3DApp::createWindow(InitializationData* initData) 
{
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

/**
 * @brief Initializes the app by calling the application's helper methods in the necessary order.
 * @param initData The data passed to the app for initialization purposes.
 */
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
	createAndBindConstantBuffer();
	createAndBindInputLayout();
}

/**
 * @brief Creates a D3D11 device.
 * @return The result of the device creation call.
 */
HRESULT D3DApp::createDeviceAndContext() 
{
	const int numFeatureLevels = 1;
	D3D_FEATURE_LEVEL supportedFeatureLevels[numFeatureLevels] = { D3D_FEATURE_LEVEL_11_1 };
	return D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, supportedFeatureLevels, numFeatureLevels, D3D11_SDK_VERSION, device.GetAddressOf(), &usedFeatureLevel, deviceContext.GetAddressOf());
}

/**
 * @brief Creates a DXGI Swapchain.
 * @return The result of the swapchain creation call.
 */
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
	swapChainDesc.BufferCount = 1;
	swapChainDesc.Windowed = initData->windowed;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.Flags = 0U;
	swapChainDesc.OutputWindow = viewportWindow;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;


	return dxgiFactory->CreateSwapChain(dxgiDevice, &swapChainDesc, swapchain.GetAddressOf());
}


/**
 * @brief Creates the render target view from the swapchain-provided backbuffer.
 * @return The result of the create render target view call.
 */
HRESULT D3DApp::createRenderTargetView() 
{
	ID3D11Texture2D* backbuffer = nullptr;
	swapchain->GetBuffer(0U, __uuidof(ID3D11Texture2D), (void**)&backbuffer);
	return device->CreateRenderTargetView(backbuffer, nullptr, renderTargetView.GetAddressOf());
}

/**
 * @brief Creates a new Texture2D resource to be used as the depth stencil texture, and its associated resource view.
 * @return The result of the create texture call if it fails, or the result of the create depth stencil view call.
 */
HRESULT D3DApp::createDepthStencilView(InitializationData* initData) 
{
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

/**
 * @brief Sets up the output merger stage of the pipeline by binding the render target and depth stencil resource views.
 */
void D3DApp::setupOMState() 
{
	deviceContext->OMSetRenderTargets(1U, renderTargetView.GetAddressOf(), depthStencilView.Get());
}

/**
 * @brief Defines and passes a viewport to the rasterizer stage of the pipeline.
 */
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

/**
 * @brief Creates and binds a vertex buffer to the input assembler stage.
 * @note The buffer is created with the DYNAMIC usage flag to allow for CPU writes to the vertex buffer.
 * @return The result of the create buffer call.
 */
HRESULT D3DApp::createAndBindVertexBuffer() 
{
	HRESULT result;
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * 1000;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.StructureByteStride = sizeof(Vertex);
	UINT strides[] = { Vertex().getStride()};
	UINT offsets[] = {0};

	if (FAILED(result = device->CreateBuffer(&vertexBufferDesc,
		0,
		vertexBuffer.GetAddressOf())))
		return result;

	deviceContext->IASetVertexBuffers(0U, 1U, vertexBuffer.GetAddressOf(), strides, offsets);
	return result;
}


/**
 * @brief Creates and binds an index buffer to the input assembler stage.
 * @note The buffer is created with the DYNAMIC usage flag to allow for CPU writes to the index buffer.
 * @return The result of the create buffer call.
 */
HRESULT D3DApp::createAndBindIndexBuffer() 
{
	HRESULT result;
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.ByteWidth = sizeof(UINT) * 1000;
	indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	indexBufferDesc.StructureByteStride = sizeof(UINT);

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
 * @return The result of the create input layout call.
 */
HRESULT D3DApp::createAndBindInputLayout() 
{
	HRESULT result;
	const UINT numDescs = 3;
	D3D11_INPUT_ELEMENT_DESC inputElementDescs[numDescs] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0U, 0U, D3D11_INPUT_PER_VERTEX_DATA, 0U},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1U, 16U, D3D11_INPUT_PER_VERTEX_DATA, 0U},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 2U, 32U, D3D11_INPUT_PER_VERTEX_DATA, 0U}
	};

	unique_ptr<char[]> byteCode = nullptr;
	SIZE_T bytecodeLength = FileReader::readAllContentsOfFile("C:/Users/Alessandro Genovese/source/repos/Direct3DApp/Debug/VertexShader.cso", &byteCode);

	if (FAILED(result = device->CreateInputLayout(inputElementDescs,
		numDescs,
		byteCode.get(),
		bytecodeLength,
		inputLayout.GetAddressOf()))) 
		return result;

	deviceContext->IASetInputLayout(inputLayout.Get());
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return result;
}


HRESULT D3DApp::createAndBindConstantBuffer() 
{
	HRESULT result;
	D3D11_BUFFER_DESC constantBufferDesc;
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.ByteWidth = sizeof(CBuffer);
	constantBufferDesc.CPUAccessFlags = 0U;
	constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	constantBufferDesc.StructureByteStride = sizeof(CBuffer);
	constantBufferDesc.MiscFlags = 0U;
	if (FAILED(result = device->CreateBuffer(&constantBufferDesc, nullptr, constantBuffer.GetAddressOf()))) return result;
	deviceContext->VSSetConstantBuffers(0U, 1U, { constantBuffer.GetAddressOf() });
	return result;
}


/**
 * @brief Binds a pixel shader to the pipeline if not already bound.
 */
void D3DApp::bindPixelShader(ID3D11PixelShader* pixelShader) 
{
	if (pixelShader != bindedPixelShader) {
		deviceContext->PSSetShader(pixelShader, nullptr, 0);
		bindedPixelShader = pixelShader;
	}
}


/**
 * @brief Binds a vertex shader to the pipeline if not already bound.
 */
void D3DApp::bindVertexShader(ID3D11VertexShader* vertexShader) 
{
	if (vertexShader != bindedVertexShader) {
		deviceContext->VSSetShader(vertexShader, nullptr, 0);
		bindedVertexShader = vertexShader;
	}
}

/**
 * @brief Attempts to bind a mesh's vertex and pixel shader to the pipeline.
 * @note A change will only occur if the binded shaders are different from this mesh's shaders.
 * @param mesh The mesh to set up the pipeline for.
 */
void D3DApp::setPipelineStateForRendering(Mesh* mesh){
	bindVertexShader(mesh->meshVertexShader->vertexShader.Get());
	bindPixelShader(mesh->meshPixelShader->pixelShader.Get());
}


/**
 * @brief Adds a mesh to the application.
 * @note By default, the application will render the mesh.
 * @param mesh The mesh to add to the application.
 */
void D3DApp::addMesh(Mesh* mesh) 
{
	meshes.push_back(mesh);
	D3D11_MAPPED_SUBRESOURCE mappedBuffer;
	deviceContext->Map(vertexBuffer.Get(), 0U, D3D11_MAP_WRITE_NO_OVERWRITE, 0U, &mappedBuffer);
	std::copy(mesh->vertices.begin(), mesh->vertices.end(), &(static_cast<Vertex*>(mappedBuffer.pData)[nextFreeVBIndex]));
	deviceContext->Unmap(vertexBuffer.Get(), 0U);

	deviceContext->Map(indexBuffer.Get(), 0U, D3D11_MAP_WRITE_NO_OVERWRITE, 0U, &mappedBuffer);
	std::copy(mesh->indices.begin(), mesh->indices.end(), &(static_cast<UINT*>(mappedBuffer.pData)[nextFreeIDBIndex]));
	deviceContext->Unmap(indexBuffer.Get(), 0U);

	mesh->startIDBIndex = nextFreeIDBIndex;
	mesh->startVBIndex = nextFreeVBIndex;

	nextFreeVBIndex += mesh->getVertexCount();
	nextFreeIDBIndex += mesh->getIndexCount();
}

/**
 * @brief Renders all the non-hidden meshes in the application.
 */
void D3DApp::render() 
{
	deviceContext->ClearRenderTargetView(renderTargetView.Get(), backgroundColour);
	deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0U);

	constBufferPerFrame.worldViewProj = camera.generateWorldViewProjMatrix(XMMatrixIdentity());
	deviceContext->UpdateSubresource(constantBuffer.Get(), 0U, nullptr, (void*)&constBufferPerFrame, 0U, 0U);

	for (Mesh* mesh : meshes) {
		setPipelineStateForRendering(mesh);
		deviceContext->DrawIndexed(mesh->getIndexCount(), mesh->startIDBIndex, mesh->startVBIndex);
	}

	swapchain->Present(0U, 0U);
}