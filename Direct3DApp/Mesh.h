#pragma once
#include "Common.h"
#include <vector>
#include "AppTypes.h"

using std::vector;

class Mesh {
public:
	UINT startIDBIndex;
	UINT startVBIndex;
	class VertexShader* meshVertexShader;
	class PixelShader* meshPixelShader;

	vector<Vertex> vertices = {};
	vector<UINT> indices = {};

	void importFromOBJ(const char* file);
	void setPixelShader(class PixelShader* ps);
	void setVertexShader(class VertexShader* vs);
	UINT getVertexCount();
	UINT getIndexCount();
};