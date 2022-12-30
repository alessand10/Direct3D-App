#include "Mesh.h"
#include <d3d11.h>
#include "D3DApp.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include <fstream>
#include <string>

using std::string;

struct OBJIndex {
	UINT index;
	UINT vnIndex;
	UINT texIndex;
	OBJIndex* next = nullptr;
	int insertMatchingIndex(UINT indexNew, UINT vnIndexNew, UINT texIndexNew, vector<Vertex>& vertices, vector<XMFLOAT4>& normals, vector<XMFLOAT2>& texCoords) {
		if (vnIndex == vnIndexNew && texIndex == texIndexNew)
			return index;
		else if (next == nullptr) {
			vertices.push_back({ vertices[index].position, normals[vnIndexNew], texCoords[texIndexNew]});
			next = new OBJIndex{ vertices.size() - 1, vnIndexNew, texIndexNew };
			return next->index;
		}
		else
			next->insertMatchingIndex(indexNew, vnIndexNew, texIndexNew, vertices, normals, texCoords);
	}

	void recursiveDestroy() {
		if (next != nullptr)
			next->recursiveDestroy();
		delete this;
	}

	bool operator==(UINT other) {
		return index == other;
	}
};

void parsePolygon(string poly, vector<OBJIndex>& objIndices, vector<XMFLOAT4> normals, vector<XMFLOAT2> texCoords, vector<UINT>& indices, vector<Vertex>& vertices) {
	size_t delimLoc = 0;
	if (poly.size() > 0)
		poly += " ";
	while (poly.size() > 0) {
		UINT index, texCoordIndex, vertexNormalIndex;
		int delimIndex = poly.find(' ');
		string token = poly.substr(0, delimIndex);
		sscanf_s(token.data(), "%d/%d/%d", &index, &texCoordIndex, &vertexNormalIndex);
		index--; texCoordIndex--; vertexNormalIndex--;
		poly = poly.erase(0U, delimIndex + 1);
		
		vector<OBJIndex>::iterator it = std::find(objIndices.begin(), objIndices.end(), index);
		if (it == objIndices.end()) {
			vertices[index].normal = normals[vertexNormalIndex];
			vertices[index].texcoord = texCoords[texCoordIndex];
			objIndices.push_back({ index, vertexNormalIndex, texCoordIndex });
			indices.push_back(index);
		}
		else {
			indices.push_back((*it).insertMatchingIndex(index, vertexNormalIndex, texCoordIndex, vertices, normals, texCoords));
		}
	}
}


/**
 * @brief Imports a mesh from a .obj file.
 * @param file The path to the file containing the mesh.
 */
void Mesh::importFromOBJ(const char* file)
{
	vector<XMFLOAT4> normals = {};
	vector<XMFLOAT2> texCoords = {};
	vector<OBJIndex> objIndices = {};

	std::ifstream fileObject(file, std::ios::in | std::ios::binary);
	std::string dump;
	Vertex lNewVertex;
	std::string lineType = "  ";

	while (1) {
		fileObject.read((char*)(lineType.data()), 2);
		
		if (lineType._Equal("v ")) {
			fileObject >> lNewVertex.position.x;
			fileObject >> lNewVertex.position.y;
			fileObject >> lNewVertex.position.z;
			vertices.push_back(lNewVertex);
			std::getline(fileObject, dump);
		}
		else if (lineType._Equal("f ")) {
			std::string line;
			std::getline(fileObject, line);
			parsePolygon(line, objIndices, normals, texCoords, indices, vertices);
		}

		else if (lineType._Equal("vt")) {
			XMFLOAT2 texCoord = {};
			fileObject >> texCoord.x;
			fileObject >> texCoord.y;
			texCoords.push_back(texCoord);
			std::getline(fileObject, dump);
		}

		else if (lineType._Equal("vn")) {
			XMFLOAT4 norm = {0.f, 0.f, 0.f, 1.f};
			fileObject >> norm.x;
			fileObject >> norm.y;
			fileObject >> norm.z;
			normals.push_back(norm);
			std::getline(fileObject, dump);
		}

		else
			std::getline(fileObject, dump);

		if (!fileObject)
			break;
	}

}


UINT Mesh::getVertexCount()
{
	return vertices.size();
}

UINT Mesh::getIndexCount()
{
	return indices.size();
}
