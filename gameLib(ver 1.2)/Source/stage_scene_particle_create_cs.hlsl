#include"stage_scene_particle.hlsli"
#include"rand_function.hlsli"
[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint deleteCount = 0;
    particleCountBuffer.InterlockedAdd(8, -1, deleteCount);
    //if (deleteCount > 0)
    {
        uint newParticleIndex = deleteIndexBuffer[deleteCount-1];
        Particle particle = (Particle) 0;
        particle.position = createCentralPosition + normalize(float3(rand_1_normal(float2(deleteCount % 325, newParticleIndex % 167), 1) - 0.5f, rand_1_normal(float2(deleteCount % 231, newParticleIndex % 237), 1) - 0.5f, rand_1_normal(float2(newParticleIndex % 521, (deleteCount + newParticleIndex) % 145), 1) - 0.5f)) * createArea;
        particle.accel = (normalize(float3(rand_1_normal(float2(deleteCount % 325, newParticleIndex % 167), 1) - randX, rand_1_normal(float2(deleteCount % 231, newParticleIndex % 237), 1), rand_1_normal(float2(newParticleIndex % 521, (deleteCount + newParticleIndex) % 145), 1))) /*+float3(0,1,0.5f)*/) * 10.f + float3(0, 2, 0);
        particle.scale = float3(1, 1, 1) * scale;
        float ratio = rand_1_normal(float2(deleteCount % 64, newParticleIndex % 37), 1);
        particle.color = lerp(color, color2, step(colorRatio.x, ratio));
        particle.life = maxLife;
        particleBuffer[newParticleIndex] = particle;
        uint particleCount = 0;

        particleCountBuffer.InterlockedAdd(0, 1, particleCount);
        indexBuffer.Store(particleCount * 4, newParticleIndex);

    }
	//uint index = DTid.x + startIndex;
	//uint bufferIndex = index * PARTICLE_MAX;

	//Particle p = (Particle)0;

	//p.position = createCentralPosition + normalize(float3(rand_1_normal(float2(index % 325, bufferIndex % 167), 1) - 0.5f, rand_1_normal(float2(index % 231, bufferIndex % 237), 1) - 0.5f, rand_1_normal(float2(bufferIndex % 521, (index + bufferIndex) % 145), 1) - 0.5f))* createArea;

	//p.accel = (normalize(float3(rand_1_normal(float2(index % 325, bufferIndex % 167), 1)- randX, rand_1_normal(float2(index % 231, bufferIndex % 237), 1), rand_1_normal(float2(bufferIndex % 521, (index + bufferIndex) % 145), 1)))/*+float3(0,1,0.5f)*/)*10.f+float3(0,2,0);

	//p.scale = float3(1, 1, 1)* scale;

	//float ratio = rand_1_normal(float2(index % 64, bufferIndex % 37), 1);
	//p.color = lerp(color, color2, step(colorRatio.x, ratio));

	//p.life = maxLife;

	//WriteParticle(p, bufferIndex);
}