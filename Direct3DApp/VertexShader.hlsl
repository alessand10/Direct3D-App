cbuffer ConstantBuffer : register(b0) {
	matrix worldViewProj;
}

struct VertDataIn {
	float4 pos : POSITION;
	float4 normal : NORMAL;
	float2 texCoord : TEXCOORD;
};

struct VertDataOut {
	float4 pos : SV_POSITION;
	float4 normal : NORMAL;
	float2 texCoord : TEXCOORD;
};

VertDataOut main(VertDataIn input)
{
	VertDataOut output;
	output.pos = mul(input.pos, worldViewProj);
	output.normal = input.normal;
	output.texCoord = input.texCoord;
	return output;
}