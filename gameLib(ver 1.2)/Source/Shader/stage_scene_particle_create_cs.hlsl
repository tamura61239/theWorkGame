#include"stage_scene_particle.hlsli"
#include"../Lib/Shaders/rand_function.hlsli"
#include"particle_count_buffer.hlsli"

/****************************************************************************/
//　　　パーティクルを生成する
/****************************************************************************/

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    //死んでるパーティクルの数を1つ減らす
    uint deleteCount = 0;
    particleCountBuffer.InterlockedAdd(8, -1, deleteCount);
    uint newParticleIndex = deleteIndexBuffer[deleteCount - 1];
    //パーティクルを生成する
    Particle particle = (Particle) 0;
    particle.position = createCentralPosition + normalize(float3(rand_1_normal(float2(deleteCount % 325, newParticleIndex % 167), 1) - 0.5f, rand_1_normal(float2(deleteCount % 231, newParticleIndex % 237), 1) - 0.5f, rand_1_normal(float2(newParticleIndex % 521, (deleteCount + newParticleIndex) % 145), 1) - 0.5f)) * createArea;
    particle.accel = (normalize(float3(rand_1_normal(float2(deleteCount % 325, newParticleIndex % 167), 1) - randX, rand_1_normal(float2(deleteCount % 231, newParticleIndex % 237), 1), rand_1_normal(float2(newParticleIndex % 521, (deleteCount + newParticleIndex) % 145), 1))) /*+float3(0,1,0.5f)*/) * 10.f + float3(0, 2, 0);
    particle.scale = float3(1, 1, 1) * scale;
    float ratio = rand_1_normal(float2(deleteCount % 64, newParticleIndex % 37), 1);
    particle.color = lerp(color, color2, step(colorRatio.x, ratio));
    particle.life = maxLife;
    //生成したパーティクルをバッファにセットする
    particleBuffer[newParticleIndex] = particle;
    //カウントを増やす
    uint particleCount = 0;
    particleCountBuffer.InterlockedAdd(0, 1, particleCount);
    particleIndexBuffer.Store(particleCount * 4, newParticleIndex);

}