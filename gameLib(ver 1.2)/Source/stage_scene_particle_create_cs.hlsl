#include"stage_scene_particle.hlsli"
#include"rand_function.hlsli"
[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	uint index = DTid.x + startIndex;
	uint bufferIndex = index * PARTICLE_MAX;

	Particle p = (Particle)0;

	p.position = createCentralPosition + normalize(float3(rand_1_normal(float2(index % 325, bufferIndex % 167), 1) - 0.5f, rand_1_normal(float2(index % 231, bufferIndex % 237), 1) - 0.5f, rand_1_normal(float2(bufferIndex % 521, (index + bufferIndex) % 145), 1) - 0.5f))* createArea;

	p.accel = (normalize(float3(rand_1_normal(float2(index % 325, bufferIndex % 167), 1)- randX, rand_1_normal(float2(index % 231, bufferIndex % 237), 1), rand_1_normal(float2(bufferIndex % 521, (index + bufferIndex) % 145), 1)))/*+float3(0,1,0.5f)*/)*10.f+float3(0,2,0);

	p.scale = float3(1, 1, 1)* scale;

	p.color = color;

	p.life = maxLife;

	WriteParticle(p, bufferIndex);
}