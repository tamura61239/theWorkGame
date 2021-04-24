struct Particle
{
    float3 position;
    float3 velocity;
    uint color;
    float scale;
    float life;
    float lifeAmoust;
};
struct Mesh
{
    float3 position;
    float3 normal;
    float2 texcoord;
    float4 boneWeight;
    uint4 boneIndex;
};
//定数バッファのデータ
cbuffer CbBone :register(b0)
{
    row_major float4x4 boneWorld[128];
}

cbuffer CbCreate : register(b1)
{
    float life;
    float speed;
    uint color;
    int indexCount;
    int startIndex;
    float3 d0;
}
cbuffer CbUpdate : register(b2)
{
    float elapsdTime;
    float3 d1;
}
//パーティクルのバッファデータ
RWStructuredBuffer<Particle> particle : register(u0);
//モデルのメッシュデータ
StructuredBuffer<Mesh> vertexBuffer : register(t0);
//モデルのindexデータ
StructuredBuffer<int> indexBuffer : register(t1);
