#include"run_particle_cs_function.hlsli"
#include"Lib/Shaders/rand_function.hlsli"
#include"particle_count_buffer.hlsli"
/****************************************************************************/
//　　　アニメーションするモデルのメッシュからパーティクルを生成する
/****************************************************************************/

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint index = DTid.x % indexCount;
    float3 pos[3] = { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } }, normal = { 0, 0, 0 };
    //メッシュの座標を取得
    for (int i = 0; i < 3;i++)
    {
        uint vertexIndex = indexBuffer[index * 3 + i];
        Mesh mesh = vertexBuffer[vertexIndex];
        for (int j = 0; j < 4;j++)
        {
            pos[i] += (mesh.boneWeight[j] * mul(float4(mesh.position, 1), boneWorld[mesh.boneIndex[j]])).xyz;
            normal += (mesh.boneWeight[j] * mul(mesh.normal, (float3x3) boneWorld[mesh.boneIndex[j]])).xyz;

        }
    }
    //取得した法線を正規化
    normal = normalize(normal);
    //死んでるパーティクルの数を1つ減らす
    uint deadCount;
    particleCountBuffer.InterlockedAdd(4 * 2, -1, deadCount);
    uint newParticleIndex = deleteIndexBuffer[deadCount - 1];
    //新しいパーティクルを生成
    Particle p = (Particle) 0;

    float3 vec1 = (pos[1] - pos[0]) * saturate(rand_1_normal(float2((newParticleIndex + deadCount) * DTid.x % 621, deadCount * 3 % 439), 0.5f));
    float3 vec2 = (pos[2] - pos[0]) * saturate(rand_1_normal(float2(newParticleIndex * 3 % 756, (newParticleIndex + deadCount) * DTid.x % 394), 0.5f));
    
    float3 vec3 = vec2 - vec1;
    
    vec3 *= saturate(rand_1_normal(float2((newParticleIndex + deadCount) * DTid.x % 567, (newParticleIndex + deadCount) * DTid.x % 381), 0.5f));
    p.position = pos[0] + vec1 + vec3;
    p.color = color;
    p.life = 1;
    p.lifeAmoust = 1 / life;
    p.scale = 0.2f;
    p.velocity = normal * speed;
    //生成したパーティクルをバッファにセットする
    particle[newParticleIndex] = p;
    //カウントを増やす
    uint aliveCount;
    particleCountBuffer.InterlockedAdd(0, 1, aliveCount);
    particleIndexBuffer.Store(aliveCount * 4, newParticleIndex);
}
