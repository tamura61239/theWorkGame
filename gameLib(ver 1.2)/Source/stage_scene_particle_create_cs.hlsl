#include"stage_scene_particle.hlsli"
[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	uint index = startIndex + DTid.x;
	uint bufferIndex = index * 19 * FLOAT_SIZE;
	Particle p;
	uint rIndex = index % 400;
	uint rBufferIndex = rIndex * 3 * FLOAT_SIZE;
	float3 rand = asfloat(randBuffer.Load3(rBufferIndex)) * index;
	rand = rand - floor(rand);
	rand.x -= 0.5f;
	p.position.xyz = startPosition + createRange * rand;
	p.position.w = 1.0f;
	p.velocity = float3(0, 75, 0);
	p.life = maxLife;
	p.angle = (float3)0;
	//p.color = float4(1, 0, 0, 0);
	p.color.rgb = normalize(float3(index % 19, index % 5, index % 23));
	p.color.w = 0;
	p.maxA = rand.x + rand.y + rand.z;
	p.maxA = p.maxA - floor(p.maxA)+0.2f;
	p.scale = float3(1 - p.maxA + 0.3f, 1 - p.maxA + 0.3f, 1 - p.maxA + 0.3f)*8;
	p.maxA *= 0.8f;

	rwBuffer.Store4(bufferIndex, asuint(p.position));
	rwBuffer.Store(bufferIndex + 4 * FLOAT_SIZE, asuint(p.life));
	rwBuffer.Store3(bufferIndex + 5 * FLOAT_SIZE, asuint(p.scale));
	rwBuffer.Store4(bufferIndex + 8 * FLOAT_SIZE, asuint(p.color));
	rwBuffer.Store3(bufferIndex + 12 * FLOAT_SIZE, asuint(p.velocity));
	rwBuffer.Store3(bufferIndex + 15 * FLOAT_SIZE, asuint(p.maxA));
	rwBuffer.Store3(bufferIndex + 16 * FLOAT_SIZE, asuint(p.angle));

}