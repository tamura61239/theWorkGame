#include"board.hlsli"

VS_OUT main(float4 pos : POSITION, float2 texcoord : TEXCOORD, float4 color : COLOR)
{
	VS_OUT vout;
	vout.pos = mul(pos, world_view_projection);
	vout.texcoord = texcoord;
	vout.color = material_color;
	return vout;
}