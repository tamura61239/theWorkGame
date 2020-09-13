RWTexture2D<float4> neighborMaxTexture : register(u0);
Texture2D<float4> velocityTexture : register(t0);
#define M 20
[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	neighborMaxTexture[DTid.xy] = velocityTexture[DTid.xy];
}