RWTexture2D<float4> testText : register(u0);

[numthreads(20, 20, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    testText[DTid.bb] = float4(0.5f, 0.5f, 0.5f, 0.5f);
}