#include"respond_particle.hlsli"
#include"particle_count_buffer.hlsli"
#include"Lib/Shaders/rand_function.hlsli"


[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    //メッシュの頂点を求める
    float3 pos[3] = { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } }, normal = { 0, 0, 0 };
    for (int i = 0; i < 3; i++)
    {
        uint vertexIndex = indexBuffer[DTid.x * 3 + i];
        Mesh mesh = vertexBuffer[vertexIndex];
        for (int j = 0; j < 4; j++)
        {
            pos[i] += mesh.boneWeight[j] * mul(float4(mesh.position, 1), bone[mesh.boneIndex[j]]).xyz;
            normal += (mesh.boneWeight[j] * mul(mesh.normal, (float3x3) bone[mesh.boneIndex[j]])).xyz;

        }
    }
    normal = normalize(normal);

    //モデルの中心を求める
    float3 modelCenter = mul(float4(0, centerY, 0, 1), bone[0]).xyz;
    //パーティクルを生成
    uint count0 = (uint)(length(pos[0] - pos[1]) / (scale * 2.5f));
    uint count1 = (uint) (length(pos[0] - pos[2]) / (scale * 2.5f));
    uint count2 = (uint) (length(pos[1] - pos[2]) / (scale * 2.5f));
    uint allCount = count0 * count1 * count2;
    if(allCount<=1)
    {
        float3 center = (pos[0] + pos[1] + pos[2]) / 3;
        //空いてる分のカウントを減らす
        uint deadCount;
        particleCountBuffer.InterlockedAdd(4 * 2, -1, deadCount);
        uint newParticleIndex = deleteIndexBuffer[deadCount - 1];
        float time = rand_1_normal(float2(deadCount % 325, newParticleIndex % 167), 0.1f) * (expansionTime * 0.4f);

        CreateParticle(center, modelCenter, normal, newParticleIndex, time,particle,particleCountBuffer,particleIndexBuffer);
    }
    else
    {
        for (int count = 1; count <= allCount;count++)
        {
            //空いてる分のカウントを減らす
            uint deadCount;
            particleCountBuffer.InterlockedAdd(4 * 2, -1, deadCount);
            uint newParticleIndex = deleteIndexBuffer[deadCount - 1];

            
            float3 vec1 = (pos[1] - pos[0]) * saturate(rand_1_normal(float2((newParticleIndex + deadCount) * count * DTid.x % 621, deadCount*allCount * 3 % 439), 0.5f));
            float3 vec2 = (pos[2] - pos[0]) * saturate(rand_1_normal(float2(newParticleIndex * 3 % 756, (newParticleIndex + deadCount) * DTid.x % 394), 0.5f));
    
            float3 vec3 = vec2 - vec1;
    
            vec3 *= saturate(rand_1_normal(float2((newParticleIndex + deadCount)*allCount * DTid.x % 567, (newParticleIndex + deadCount)*count * DTid.x % 381), 0.5f));

            float3 position = pos[0] + vec1 + vec3;
            float time = rand_1_normal(float2(deadCount % 325, newParticleIndex % 167), 0.1f) * (expansionTime * 0.4f);

            CreateParticle(position, modelCenter, normal, newParticleIndex, time, particle, particleCountBuffer, particleIndexBuffer);

        }

    }


}