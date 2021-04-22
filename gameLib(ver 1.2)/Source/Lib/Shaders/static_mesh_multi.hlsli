struct VS_OUT
{
	float4 position:SV_POSITION;
	float4 worldPosition:POSITION;
	float4 color:COLOR;
	float3 worldNormal:TEXCOORD0;
	float2 texcoord:TEXCOORD1;
};

struct PS_OUT
{
	float4 color:SV_TARGET0;
	float4 position:SV_TARGET1;
	float4 normal:SV_TARGET2;
};

cbuffer CbScene:register(b0)
{
	float4 lightColor;
	float4 lightDirection;
	float4 ambientColor;
	float4 eyePosition;
	row_major float4x4 view;
	row_major float4x4 projection;
};

cbuffer CbObj:register(b1)
{
	float4 materialColor;
	row_major float4x4 world;
};
