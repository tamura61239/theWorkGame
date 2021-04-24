#include "model.hlsli"
/*****************************************************/
//　　　　	頂点座標をスクリーン空間系に計算する
/*****************************************************/

VS_OUT main(
	float4 position     : POSITION,
	float3 normal : NORMAL,
	float2 texcoord : TEXCOORD,
	float4 boneWeights : WEIGHTS,
	uint4  boneIndices : BONES
)
{
	float3 p = { 0, 0, 0 };
	float3 n = { 0, 0, 0 };
	for (int i = 0; i < 4; i++)
	{
		p += (boneWeights[i] * mul(position, boneTransforms[boneIndices[i]])).xyz;
		n += (boneWeights[i] * mul(normal, (float3x3)boneTransforms[boneIndices[i]])).xyz;
	}

	VS_OUT vout;
	vout.worldPosition = p;
    vout.position = mul(float4(p, 1.0f), mul(view, projection));

	float3 N = normalize(n);
	vout.worldNormal = N;
	float3 vB = normalize(float3(0, 1, -0.001f));
	float3 vT = normalize(cross(N, vB));
	vB = normalize(cross(N, vT));
	vout.vT = vT;
	vout.vB = vB;
	vout.texcoord = texcoord;
	vout.color = materialColor;

	return vout;
}
