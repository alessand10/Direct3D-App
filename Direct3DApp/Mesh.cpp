#include "Mesh.h"
#include <d3d11.h>
#include "D3DApp.h"
#include "VertexShader.h"
#include "PixelShader.h"

void Mesh::importFromOBJ(const char* file)
{

}

void Mesh::setPipelineStateForRendering(D3DApp* app)
{
	app->bindVertexShader(meshVertexShader->vertexShader.Get());
	app->bindPixelShader(meshPixelShader->pixelShader.Get());
}
