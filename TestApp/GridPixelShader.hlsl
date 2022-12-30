struct VertDataOut
{
    float4 pos : SV_POSITION;
    float4 normal : NORMAL;
    float2 texCoord : TEXCOORD;
};

float4 main(VertDataOut dataOut) : SV_TARGET
{
    int2 xyGridLines = int2(5, 5);
	
    float2 scaledCoords = dataOut.texCoord * xyGridLines;
    float2 num = abs(frac(scaledCoords) - 0.5f) * -1.f + 0.5f;
    float2 denom = fwidth(dataOut.texCoord) * xyGridLines;
    return length(1.f - saturate(num / (2.f * denom)));
}