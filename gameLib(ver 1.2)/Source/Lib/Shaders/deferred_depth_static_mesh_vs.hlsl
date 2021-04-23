#include "deferred_depth_static_mesh.hlsli"
/*****************************************************/
//　　　　頂点の座標をスクリーン座標系にする
/*****************************************************/

VS_OUT main(float4 position : POSITION, float3 normal : NORMAL, float2 texcoord : TEXCOORD/*UNIT.13*/)
{
	VS_OUT vout;
	vout.position = mul(float4(position.xyz,1), world);
	vout.worldPosition = vout.position;
	vout.position = mul(vout.position, view);
	vout.position = mul(vout.position, projection);
	vout.depth = vout.position;
	vout.worldNormal = normalize(mul(normal, (float3x3)world));
	vout.color = materialColor;

	vout.texcoord = texcoord;

	return vout;
}
