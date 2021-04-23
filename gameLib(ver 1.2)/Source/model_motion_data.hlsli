struct VS_OUT
{
    float4 position : POSITION0;
    float4 beforePosition : POSITION1;
};
struct GS_OUT
{
    float4 position : SV_POSITION;
    float3 velocity : TEXCOORD;
};
//定数バッファのデータ
cbuffer CbScene : register(b0)
{
    row_major float4x4 viewProjection;
};
#define MAX_BONES 128
cbuffer CbMesh : register(b1)
{
    row_major float4x4 boneTransforms[MAX_BONES];
    row_major float4x4 beforeBoneTransforms[MAX_BONES];

};
