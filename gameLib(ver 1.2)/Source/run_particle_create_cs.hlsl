#include"run_particle_cs_function.hlsli"
#include"rand_function.hlsli"

/********************初期化用コンピュートシェーダー*********************/
[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint index = DTid.x % indexCount;
    float3 pos[3] = { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } }, normal = { 0, 0, 0 };
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
    normal = normalize(normal);
    Particle p = (Particle) 0;

    float3 vec1 = (pos[1] - pos[0]) * saturate(rand_1_normal(float2((startIndex + DTid.x) % 621, DTid.x * 3 % 439), 0.5f));
    float3 vec2 = (pos[2] - pos[0]) * saturate(rand_1_normal(float2(DTid.x * 3 % 756, (startIndex + DTid.x) % 394), 0.5f));
    
    float3 vec3 = vec2 - vec1;
    
    vec3 *= saturate(rand_1_normal(float2((startIndex + DTid.x) % 567, (startIndex + DTid.x) % 381), 0.5f));
    p.position = pos[0] + vec1 + vec3;
    p.color = color;
    p.life = 1;
    p.lifeAmoust = 1 / life;
    p.scale = 0.2f;
    p.velocity = normal;
    particle[startIndex+DTid.x] = p;
    
}