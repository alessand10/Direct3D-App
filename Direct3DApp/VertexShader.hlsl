cbuffer ConstantBuffer : register(b0) {
	matrix worldViewProj;
}

float4 main(float4 pos : POSITION, float4 normal : NORMAL, float2 texCoord : TEXCOORD) : SV_POSITION
{
	return mul(pos, worldViewProj);
}