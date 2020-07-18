struct VS_OUT
{
	float4 position:SV_POSITION;
	float4 color:COLOR0;
	float3 worldNormal:TEXCOORD1;
	float3 worldPosition:TEXCOORD2;
	float2 texcoord:TEXCOORD3;
	float3 vT:TEXCOORD4;
	float3 vB:TEXCOORD5;
};
cbuffer CbScene : register(b0)
{
	float4 lightColor;
	float4 lightDirection;
	float4 ambientColor;
	float4 eyePosition;
	row_major float4x4	viewProjection;
};
#define MAX_BONES 128
cbuffer CbMesh : register(b1)
{
	row_major float4x4	boneTransforms[MAX_BONES];
};

cbuffer CbSubset : register(b2)
{
	float4				materialColor;
};
