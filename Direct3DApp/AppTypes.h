#pragma once
#include "Common.h"

using DirectX::XMFLOAT3;
using DirectX::XMFLOAT4;
using DirectX::XMMATRIX;

struct Vertex {
	XMFLOAT4 position = { 0.f, 0.f, 0.f, 1.f };
	XMFLOAT4 normal = { 0.f, 0.f, 0.f, 1.f };
	XMFLOAT2 texcoord = { 0.f, 0.f };
	UINT getStride() { return sizeof(XMFLOAT4) + sizeof(XMFLOAT4) + sizeof(XMFLOAT2); }
};

struct CBuffer {
	XMMATRIX worldViewProj = DirectX::XMMatrixIdentity();
};
