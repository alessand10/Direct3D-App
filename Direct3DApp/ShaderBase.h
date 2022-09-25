#pragma once
#include "Common.h"
#include <d3d11.h>

class ShaderBase {
protected:
	SIZE_T importShaderBytecode(const char* filePath, char** output);
};