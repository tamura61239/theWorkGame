struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 texcoord : TEXCOORD;
	float4 color : COLOR;
};
//定数バッファ
cbuffer CONSTANT_BUFFER:register(b0)
{
	row_major float4x4 world_view_projection;
	float4 material_color;
};
