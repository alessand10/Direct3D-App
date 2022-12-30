#pragma once
#include "Common.h"
#include <vector>
#include "AppTypes.h"

using std::vector;

class Mesh {
public:
	UINT startIDBIndex;
	UINT startVBIndex;

	vector<Vertex> vertices = {};
	vector<UINT> indices = {};

	void importFromOBJ(const char* file);
	UINT getVertexCount();
	UINT getIndexCount();
};