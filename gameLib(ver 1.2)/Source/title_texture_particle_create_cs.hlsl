#include"title_texture_particle.hlsli"
Texture2D tex:register(t0);
[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	uint index = DTid.x + startIndex;
	uint bufferIndex = index * MAX;
	uint2 uv = (DTid.x % textureSize.x, DTid.x / textureSize.x);
	Particle p = (Particle)0;
	p.color = float4(1,0,0,1);
	float4 pos = float4(uv.x + leftTop.x, uv.y + leftTop.y, z,1);
	pos.xy = uv / viewport;
	float4 ndcPosition = (float4)0;
	ndcPosition.x = pos.x * 2 - 1;
	ndcPosition.y = pos.y * -2 - 1;
	ndcPosition.z = pos.z;
	ndcPosition.w = pos.w;
	float4 worldPosition = mul(ndcPosition, inverseViewProjection);
	worldPosition /= worldPosition.w;
	p.position = float3(pos.xy*20,-100);
	WriteParticle(p, bufferIndex);
}