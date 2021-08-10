struct VS_OUT
{
	float4 position: SV_POSITION;
	float3 worldPosition:POSITION;
	float3 scale:SCALE;
	float3x3 rotateMatrix:ROTATE;
	float4 color:COLOR;
};
struct GS_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};
cbuffer CbScene:register(b0)
{
	row_major float4x4 view;
	row_major float4x4 projection;
}
