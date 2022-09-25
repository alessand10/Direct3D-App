#pragma once
#include <d3d11.h>
#include <vector>
#include "Common.h"

using std::vector;

class ShaderManager {
	ID3D11Device* deviceRef;
	vector<ComPtr<ID3D11VertexShader>> vertexShaders;
	vector<ComPtr<ID3D11GeometryShader>> geometryShaders;
	vector<ComPtr<ID3D11ComputeShader>> computeShaders;
	vector<ComPtr<ID3D11PixelShader>> pixelShaders;

	enum class Type {
		VS, GS, CS, PS
	};

public:
	int createShader(Type type, const char* file);
};
