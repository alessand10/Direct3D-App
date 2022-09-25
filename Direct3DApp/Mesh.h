#pragma once
#include "Common.h"
#include <vector>
#include "AppTypes.h"

using std::vector;

class Mesh {
public:
	UINT startIndex;
	UINT indexCount;
	class VertexShader* meshVertexShader;
	class PixelShader* meshPixelShader;

	vector<Vertex> vertices = {};
	vector<UINT> indices = {};

	void importFromOBJ(const char* file);
	void setPipelineStateForRendering(class D3DApp* app);
};