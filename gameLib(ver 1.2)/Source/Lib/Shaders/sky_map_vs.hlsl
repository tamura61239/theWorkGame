#include"sky_map.hlsli"
/*****************************************************/
//　　　　	頂点座標をスクリーン空間系に計算する
/*****************************************************/

VS_OUT main(float4 position : POSITION, float3 normal : NORMAL)
{
	VS_OUT vout;
	vout.position = mul(position, mul(world, mul(view, projection)));
	vout.texcoord = normalize(position.xyz);
	return vout;
}