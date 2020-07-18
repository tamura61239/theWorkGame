struct VS_OUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};
cbuffer CbScene:register(b0)
{
	row_major float4x4 view;
	row_major float4x4 projection;
};
cbuffer CbObj : register(b1)
{
	row_major float4x4 world;
};