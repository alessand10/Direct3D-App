#pragma once
#include "Common.h"

using DirectX::XMFLOAT3;
using DirectX::XMFLOAT4;
using DirectX::XMMATRIX;

struct Vertex {
	XMFLOAT4 position = {0.f, 0.f, 0.f, 1.f};
};

struct CBuffer {
	XMMATRIX worldViewProj = DirectX::XMMatrixIdentity();
};
