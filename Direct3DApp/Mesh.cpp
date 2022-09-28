#include "Mesh.h"
#include <d3d11.h>
#include "D3DApp.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include <fstream>
#include <string>

/**
 * @brief Imports a mesh from a .obj file.
 * @param file The path to the file containing the mesh.
 */
void Mesh::importFromOBJ(const char* file)
{
	std::ifstream fileObject(file, std::ios::in | std::ios::binary);
	std::string dump;
	Vertex lNewVertex;
	UINT newIndex;
	bool exit = false;
	char lineType = '\0';

	while (!exit) {
		fileObject >> lineType;
		switch (lineType) {
		case 'v':
			fileObject >> lNewVertex.position.x;
			fileObject >> lNewVertex.position.y;
			fileObject >> lNewVertex.position.z;
			vertices.push_back(lNewVertex);
			break;

		case 'f':
			for (int indexNum = 0; indexNum < 3; indexNum++) {
				fileObject >> newIndex;
				indices.push_back(newIndex - 1);
			}
			break;
		case '\0':
			exit = true;
			break;
		}
		lineType = '\0';
		std::getline(fileObject, dump);
	}
}

/**
 * @brief Sets the pixel shader to be used for this mesh.
 * @param ps The pixel shader to use.
 */
void Mesh::setPixelShader(PixelShader* ps) {
	meshPixelShader = ps;
}

/**
 * @brief Sets the vertex shader to be used for this mesh.
 * @param vs The vertex shader to use.
 */
void Mesh::setVertexShader(VertexShader* vs) {
	meshVertexShader = vs;
}

UINT Mesh::getVertexCount()
{
	return vertices.size();
}

UINT Mesh::getIndexCount()
{
	return indices.size();
}
