#include "static_mesh.hlsli"
/*****************************************************/
//　　　　	頂点座標をスクリーン空間系に計算する
/*****************************************************/

VS_OUT main(float4 position : POSITION, float3 normal : NORMAL, float2 texcoord : TEXCOORD/*UNIT.13*/)
{
	VS_OUT vout;
	vout.position = mul(float4(position.xyz,1.0f), world);
	vout.worldPosition = vout.position;
	vout.position = mul(vout.position, view);
	vout.position = mul(vout.position, projection);

	vout.worldNormal = normalize(mul(normal, (float3x3)world));
	vout.color = materialColor;

	vout.texcoord = texcoord;

	return vout;
}
