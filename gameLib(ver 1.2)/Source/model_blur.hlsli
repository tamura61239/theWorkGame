struct VS_OUT
{
	float4 position:SV_POSITION;
	float4 velocity:VELOCITY;
};

cbuffer CbScene : register(b0)
{
	row_major float4x4	viewProjection;
};
#define MAX_BONES 128
cbuffer CbMesh : register(b1)
{
	row_major float4x4	boneTransforms[MAX_BONES];
};
cbuffer CbBeforeMesh:register(b6)
{
	row_major float4x4	beforeBoneTransforms[MAX_BONES];
};
cbuffer CbBeforeScene:register(b5)
{
	row_major float4x4 beforeView;
	row_major float4x4 beforeProjection;
};

