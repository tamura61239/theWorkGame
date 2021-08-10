#include"stage_scene_particle.hlsli"
#include"../Lib/Shaders/rand_function.hlsli"
#include"particle_count_buffer.hlsli"

/****************************************************************************/
//�@�@�@�p�[�e�B�N���𐶐�����
/****************************************************************************/

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    //����ł�p�[�e�B�N���̐���1���炷
    uint deleteCount = 0;
    particleCountBuffer.InterlockedAdd(8, -1, deleteCount);
    uint newParticleIndex = deleteIndexBuffer[deleteCount - 1];
    //�p�[�e�B�N���𐶐�����
    Particle particle = (Particle) 0;
    particle.position = createCentralPosition + normalize(float3(rand_1_normal(float2(deleteCount % 325, newParticleIndex % 167), 1) - 0.5f, rand_1_normal(float2(deleteCount % 231, newParticleIndex % 237), 1) - 0.5f, rand_1_normal(float2(newParticleIndex % 521, (deleteCount + newParticleIndex) % 145), 1) - 0.5f)) * createArea;
    particle.accel = (normalize(float3(rand_1_normal(float2(deleteCount % 325, newParticleIndex % 167), 1) - randX, rand_1_normal(float2(deleteCount % 231, newParticleIndex % 237), 1), rand_1_normal(float2(newParticleIndex % 521, (deleteCount + newParticleIndex) % 145), 1))) /*+float3(0,1,0.5f)*/) * 10.f + float3(0, 2, 0);
    particle.scale = float3(1, 1, 1) * scale;
    float ratio = rand_1_normal(float2(deleteCount % 64, newParticleIndex % 37), 1);
    particle.color = lerp(color, color2, step(colorRatio.x, ratio));
    particle.life = maxLife;
    //���������p�[�e�B�N�����o�b�t�@�ɃZ�b�g����
    particleBuffer[newParticleIndex] = particle;
    //�J�E���g�𑝂₷
    uint particleCount = 0;
    particleCountBuffer.InterlockedAdd(0, 1, particleCount);
    particleIndexBuffer.Store(particleCount * 4, newParticleIndex);

}