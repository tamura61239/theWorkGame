#include"static_mesh_normal.hlsli"
/*****************************************************/
//　　　　	頂点座標をスクリーン空間系に計算する
/*****************************************************/

VS_OUT main(float4 position : POSITION, float3 normal : NORMAL, float2 texcoord : TEXCOORD)
{
	VS_OUT vout;
	vout.position = mul(position, world);
	vout.worldPosition = vout.position.xyz;
	vout.position = mul(vout.position, view);
	vout.position = mul(vout.position, projection);

	float3 wN = normalize(mul(normal, (float3x3)world));
	vout.worldNormal = wN;
	vout.color = materialColor;

	vout.texcoord = texcoord;

	float3 vN = wN;
	//float3 vT = normalize(cross(wN, float3(0, 1, 0)));
	//float3 vB = cross(vN, vT);
	float3 vB = normalize(float3(0, 1, -0.001f));
	float3 vT = normalize(cross(vB, vN));
	vB = normalize(cross(vN, vT));
	//接空間軸
	vout.vT = vT;
	vout.vB = vB;

	return vout;
}