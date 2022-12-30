#include "D3DApp.h"
#include <d3d11.h>
#include "AppTypes.h"
#include "ViewportWindowProc.h"
#include "FileReader.h"
#include "AppTimer.h"
#include <string>
#include <math.h>
#include <WICTextureLoader.h>
#include <codecvt>
#include "Utilities.h"

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
	viewportClass.lpfnWndProc = ViewportWndProc;
	viewportClass.hIcon = LoadIconW(initData->hInstance, NULL);
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
	appRef = this;
}


ComPtr<ID3D11Texture2D> D3DApp::createTextureFromFile(string path) {
	ComPtr<ID3D11Texture2D> textureResource;
	ComPtr<ID3D11ShaderResourceView> resourceView;
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	CreateWICTextureFromFile(device.Get(), converter.from_bytes(path).c_str(), reinterpret_cast<ID3D11Resource**>(textureResource.GetAddressOf()), &resourceView);
	textures.push_back(textureResource);
	return textures.back();
}

ComPtr<ID3D11Texture2D> D3DApp::createTexture(D3D11_TEXTURE2D_DESC* texDesc, D3D11_SUBRESOURCE_DATA* initialData)
{
	ComPtr<ID3D11Texture2D> textureResource;
	device->CreateTexture2D(texDesc, initialData, textureResource.GetAddressOf());
	textures.push_back(textureResource);
	return textures.back();
}

ComPtr<ID3D11Texture2D> D3DApp::createTexture(UINT16 width, UINT16 height, DXGI_FORMAT fmt, D3D11_USAGE usage, UINT bindFlags, D3D11_SUBRESOURCE_DATA* initialData)
{
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Format = fmt;
	texDesc.ArraySize = 1U;
	texDesc.BindFlags = bindFlags;
	texDesc.Height = height;
	texDesc.Width = width;
	texDesc.SampleDesc.Count = 1U;
	texDesc.SampleDesc.Quality = 0U;
	texDesc.MiscFlags = 0U;
	texDesc.Usage = usage;
	texDesc.MipLevels = 1U;
	texDesc.CPUAccessFlags = 0U;

	switch (usage) {
	case D3D11_USAGE_STAGING:
		texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	case D3D11_USAGE_DYNAMIC:
		texDesc.CPUAccessFlags |= D3D11_CPU_ACCESS_WRITE;
		break;
	default:
		texDesc.CPUAccessFlags = 0U;
	}

	return createTexture(&texDesc, initialData);
}

ComPtr<ID3D11UnorderedAccessView> D3DApp::createUAV(ComPtr<ID3D11Resource> resource)
{
	ComPtr<ID3D11UnorderedAccessView> uav = nullptr;
	device->CreateUnorderedAccessView(resource.Get(), nullptr, uav.GetAddressOf());
	uavs.push_back(uav);
	return uavs.back();
}

ComPtr<ID3D11ShaderResourceView> D3DApp::createSRV(ComPtr<ID3D11Resource> resource)
{
	ComPtr<ID3D11ShaderResourceView> srv = nullptr;
	device->CreateShaderResourceView(resource.Get(), nullptr, srv.GetAddressOf());
	srvs.push_back(srv);
	return srvs.back();
}

void D3DApp::setComputeShader(ComPtr<ID3D11ComputeShader> cs)
{
	deviceContext->CSSetShader(cs.Get(), nullptr, 0U);
}

void D3DApp::setPixelShader(ComPtr<ID3D11PixelShader> ps)
{
	deviceContext->PSSetShader(ps.Get(), nullptr, 0U);
}

void D3DApp::setVertexShader(ComPtr<ID3D11VertexShader> vs)
{
	deviceContext->VSSetShader(vs.Get(), nullptr, 0U);
}

void D3DApp::beginRendering()
{
	timer.setPreRenderTime();
	deviceContext->ClearRenderTargetView(renderTargetView.Get(), backgroundColour);
	deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0U);
	constBufferPerFrame.worldViewProj = camera.generateWorldViewProjMatrix(XMMatrixIdentity());
	deviceContext->UpdateSubresource(constantBuffer.Get(), 0U, nullptr, (void*)&constBufferPerFrame, 0U, 0U);
}

void D3DApp::endRendering()
{
	timer.setPostRenderTime();
	if(settings.lockMaxFPS) Sleep(timer.getPostRenderSleepTime(settings.maxFpsLock));
	swapchain->Present(0U, 0U);
}

void D3DApp::renderMesh(Mesh* mesh)
{
	deviceContext->DrawIndexed(mesh->getIndexCount(), mesh->startIDBIndex, mesh->startVBIndex);
}

/**
 * @brief Creates a D3D11 device.
 * @return The result of the device creation call.
 */
HRESULT D3DApp::createDeviceAndContext() 
{
	const int numFeatureLevels = 1;
	D3D_FEATURE_LEVEL supportedFeatureLevels[numFeatureLevels] = { D3D_FEATURE_LEVEL_11_1 };

	return D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE,
		NULL, D3D11_CREATE_DEVICE_DEBUG,
		supportedFeatureLevels, numFeatureLevels,
		D3D11_SDK_VERSION, device.GetAddressOf(),
		&usedFeatureLevel, deviceContext.GetAddressOf());
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
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 144U;
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

	// Create the texture for depth/stencil testing.
	HRESULT result = device->CreateTexture2D(&depthStencilTexDesc, nullptr, depthStencilTex.GetAddressOf());
	if (FAILED(result)) return result;
	
	// Create the associated resource view to bind to the output merger stage.
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
	d3dViewport.Width = static_cast<float>(initData->resolution[0]);
	d3dViewport.Height = static_cast<float>(initData->resolution[1]);
	d3dViewport.MaxDepth = 1.0f;
	d3dViewport.MinDepth = 0.0f;
	d3dViewport.TopLeftX = static_cast<float>(initData->position[0]);
	d3dViewport.TopLeftY = static_cast<float>(initData->position[1]);

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
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * 3000;
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
	indexBufferDesc.ByteWidth = sizeof(UINT) * 6000;
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
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0U, 16U, D3D11_INPUT_PER_VERTEX_DATA, 0U},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0U, 32U, D3D11_INPUT_PER_VERTEX_DATA, 0U}
	};

	unique_ptr<char[]> byteCode = nullptr;
	SIZE_T bytecodeLength = FileReader::readAllContentsOfFile("../Debug/VertexShader.cso", &byteCode);

	if (FAILED(result = device->CreateInputLayout(inputElementDescs,
		numDescs,
		byteCode.get(),
		bytecodeLength,
		inputLayout.GetAddressOf()))) 
		return result;

	// Delete bytecode after using it for the input layout.
	byteCode.reset();
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
 * @brief Adds a mesh to the application.
 * @note By default, the application will render the mesh.
 * @param mesh The mesh to add to the application.
 */
void D3DApp::addMesh(Mesh* mesh) 
{
	meshes.push_back(mesh);
	D3D11_MAPPED_SUBRESOURCE mappedBuffer;

	// Write new vertices from mesh into vertex buffer starting from the next available space in memory (nextFreeVBIndex).
	deviceContext->Map(vertexBuffer.Get(), 0U, D3D11_MAP_WRITE_NO_OVERWRITE, 0U, &mappedBuffer);
	std::copy(mesh->vertices.begin(), mesh->vertices.end(), &(static_cast<Vertex*>(mappedBuffer.pData)[nextFreeVBIndex]));
	deviceContext->Unmap(vertexBuffer.Get(), 0U);

	// Write new indices from mesh into index buffer starting from the next available space in memory (nextFreeIDBIndex).
	deviceContext->Map(indexBuffer.Get(), 0U, D3D11_MAP_WRITE_NO_OVERWRITE, 0U, &mappedBuffer);
	std::copy(mesh->indices.begin(), mesh->indices.end(), &(static_cast<UINT*>(mappedBuffer.pData)[nextFreeIDBIndex]));
	deviceContext->Unmap(indexBuffer.Get(), 0U);

	// Provide the mesh with its location within the index/vertex buffers.
	mesh->startIDBIndex = nextFreeIDBIndex;
	mesh->startVBIndex = nextFreeVBIndex;

	nextFreeVBIndex += mesh->getVertexCount();
	nextFreeIDBIndex += mesh->getIndexCount();
}


/**
 * @brief Handles the mouse move message passed by the windows procedure.
 * @note The mousePrevX and mousePrevY variables are used to determine the mouse delta position.
 * @param x The new mouse x position.
 * @param y The new mouse y position.
 */
int mousePrevX = -1, mousePrevY = -1;
void D3DApp::moveMouse(int x, int y, WPARAM wParam) {
	float mouseDelta[2] = { mousePrevX - x , y - mousePrevY };
	switch (wParam)
	{
	case MK_SHIFT | MK_MBUTTON:
		panViewport(0.01f * mouseDelta[0], 0.01f * mouseDelta[1]);
		break;

	// Middle button is down
	case MK_MBUTTON:
		rotateViewport(0.01f * mouseDelta[0], 0.01f * mouseDelta[1]);
		break;

	default:
		break;
	}

	mousePrevX = x;
	mousePrevY = y;
}

/**
 * @brief Rotates the viewport camera around its view target.
 * @note The derivation of this is shown in the github project under "ViewportRotDerivation.png"
 * @param newHAngle The angle added to the camera's horizontal angle.
 * @param newVAngle The angle added to the camera's vertical angle.
 */
void D3DApp::rotateViewport(float newHAngle, float newVAngle) {
	XMFLOAT4 camLocWS = camera.getLocation();
	XMFLOAT4 camTargetWS = camera.getLookAtTarget();
	VEC_TO_F4(camLocLocal, (F4_TO_VEC(camLocWS) - F4_TO_VEC(camTargetWS)));

	newHAngle += atanf(camLocLocal.z / camLocLocal.x);
	newHAngle += (camLocLocal.x < 0.f ? TMath::pi : 0.f);

	newVAngle += atanf(camLocLocal.y / sqrtf(powf(camLocLocal.x, 2.f) + powf(camLocLocal.z, 2.f)));
	newVAngle = TMath::clamp(TMath::truncate(-TMath::pi_div2, 2), TMath::truncate(TMath::pi_div2, 2), newVAngle);

	float horizontalDistance = abs(camera.getDistanceFromTarget() * cosf(newVAngle));
	XMVECTOR newCameraLoc = { cosf(newHAngle) , tanf(newVAngle), sinf(newHAngle) };
	newCameraLoc *= horizontalDistance;
	newCameraLoc += F4_TO_VEC(camTargetWS);
	VEC_TO_F4(result, newCameraLoc);

	camera.setLocation(result);
}


/**
 * @brief Pans the viewport camera.
 * @param x The change along the camera's local x axis.
 * @param y The change along the camera's local y axis;
 */
void D3DApp::panViewport(float x, float y) {
	XMFLOAT4 rightVec = camera.getRightVector();
	XMFLOAT4 upVec = camera.getUpVector();
	VEC_TO_F4(translationVec, -x * F4_TO_VEC(rightVec) + y * F4_TO_VEC(upVec));
	camera.panCamera(translationVec);
}

/**
 * @brief Handles input from the scroll wheel by zooming the viewport camera.
 * @param scrollMultiple The scroll wheel multiple passed in by the windows procedure.
 */
void D3DApp::scrollMouse(int scrollMultiple) {
	XMFLOAT4 camLocWS = camera.getLocation();
	XMFLOAT4 camTargetWS = camera.getLookAtTarget();
	VEC_TO_F4(camLocLocal, (F4_TO_VEC(camLocWS) - F4_TO_VEC(camTargetWS)));

	if (scrollMultiple != 0) {
		XMVECTOR vec = F4_TO_VEC(camLocLocal) * (scrollMultiple < 0 ? zoomMultiple : 1.f/zoomMultiple);
		VEC_TO_F4(result, vec + F4_TO_VEC(camTargetWS));
		camera.setLocation(result);
	}
}

/**
 * @brief Sends the camera location and view target back to their default startup locations.
 */
void D3DApp::centerViewport() {
	camera.setLocation(defaultCamLoc);
	camera.setLookAtTarget(defaultCamViewTarget);
}


void D3DApp::bindSRV(ShaderTypeSRV type, ComPtr<ID3D11ShaderResourceView> srv, UINT slot)
{
	switch (type) {
	case ShaderTypeSRV::VS:
		deviceContext->VSSetShaderResources(slot, 1U,srv.GetAddressOf());
		break;

	case ShaderTypeSRV::CS:
		deviceContext->CSSetShaderResources(slot, 1U, srv.GetAddressOf());
		break;

	case ShaderTypeSRV::PS:
		deviceContext->PSSetShaderResources(slot, 1U, srv.GetAddressOf());
		break;

	default:

		break;
	}
}

void D3DApp::bindUAV(ShaderTypeUAV type, ComPtr<ID3D11UnorderedAccessView> uav, UINT slot)
{
	switch (type) {
	case ShaderTypeUAV::CS:
		UINT val = 0;
		deviceContext->CSSetUnorderedAccessViews(slot, 1U, uav.GetAddressOf(), NULL);
		break;
	}
}


void D3DApp::dispatchComputeShader(UINT threadGroupCountX, UINT threadGroupCountY, UINT threadGroupCountZ)
{
	deviceContext->Dispatch(threadGroupCountX, threadGroupCountY, threadGroupCountZ);
}


D3DApp* appRef = nullptr;