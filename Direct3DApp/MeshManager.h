#pragma once
#include <DoubleLinkedList.h>
#include "Common.h"


class MeshManager {
	DoubleLinkedList<unique_ptr<class Mesh>> meshes;
	
public:
	class Mesh* importMeshFromOBJ(const char* file);
};