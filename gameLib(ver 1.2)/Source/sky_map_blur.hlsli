struct VS_OUT
{
	float4 position:SV_POSITION;
	float4 velocity:VELOCITY;
};
cbuffer CbScene:register(b0)
{
	row_major float4x4 view;
	row_major float4x4 projection;
};

cbuffer CbObj:register(b1)
{
	float4 materialColor;
	row_major float4x4 world;
};
cbuffer CbBeforeObj:register(b2)
{
	row_major float4x4 beforeWorld;
};
cbuffer CbBeforeScene:register(b5)
{
	row_major float4x4 beforeView;
	row_major float4x4 beforeProjection;
};
