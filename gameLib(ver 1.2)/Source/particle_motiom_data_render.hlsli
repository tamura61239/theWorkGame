struct VS_OUT
{
    float4 position : SV_POSITION;
    float3 worldPosition : POSITION;
    float3 scale : SCALE;
    float3x3 rotateMatrix : ROTATE;
    float4 color : COLOR;
    float3 velocity : VELOCITY;
};
struct GS_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};
struct PS_OUT
{
    float4 color : SV_TARGET0;
    float4 velocity : SV_TARGET1;
};
cbuffer CbScene : register(b0)
{
    row_major float4x4 view;
    row_major float4x4 projection;
}
