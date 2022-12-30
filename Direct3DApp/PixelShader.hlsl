struct VertDataOut {
	float4 pos : SV_POSITION;
	float4 normal : NORMAL;
	float2 texCoord : TEXCOORD;
};

Texture2D tex : register(t3);

SamplerState MeshTextureSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

float4 main(VertDataOut dataOut) : SV_TARGET
{
    return tex.Sample(MeshTextureSampler, dataOut.texCoord);
}