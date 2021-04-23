struct VS_OUT
{
	float4 position:SV_POSITION;
	float3 texcoord:TEXCOORD;
};
//定数バッファのデータ
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
