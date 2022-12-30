#pragma once
#include "Common.h"
#include <vector>

using DirectX::XMFLOAT3;
using DirectX::XMFLOAT4;
using DirectX::XMMATRIX;

#define T_SRV ComPtr<struct ID3D11ShaderResourceView>
#define T_UAV ComPtr<struct ID3D11UnorderedAccessView>
#define T_TEX2D ComPtr<struct ID3D11Texture2D>

struct Vertex {
	XMFLOAT4 position = { 0.f, 0.f, 0.f, 1.f };
	XMFLOAT4 normal = { 0.f, 0.f, 0.f, 1.f };
	XMFLOAT2 texcoord = { 0.f, 0.f };
	static UINT getStride() { return sizeof(XMFLOAT4) + sizeof(XMFLOAT4) + sizeof(XMFLOAT2); }
};

struct Polygon {
	std::vector<Vertex*> vertices = {};

};

struct CBuffer {
	XMMATRIX worldViewProj = DirectX::XMMatrixIdentity();
};

struct Resolution {
	UINT16 x;
	UINT16 y;
};

enum class ShaderTypeSRV {
	VS,
	PS,
	CS
};

enum class ShaderTypeUAV {
	PS,
	CS
};

struct AppSettings {
	bool lockMaxFPS = false;
	float maxFpsLock = 120.f;
};

enum class View {
	SRV,
	UAV
};
