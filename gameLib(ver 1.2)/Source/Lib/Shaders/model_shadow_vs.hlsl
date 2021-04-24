#include"model.hlsli"
/*****************************************************/
//　　　　	頂点座標をスクリーン空間系に計算する
/*****************************************************/

float4 main(
	float4 position     : POSITION,
	float3 normal : NORMAL,
	float2 texcoord : TEXCOORD,
	float4 boneWeights : WEIGHTS,
	uint4  boneIndices : BONES
) : SV_POSITION
{
	float4 p = { 0, 0, 0, 0 };
    for (int i = 0; i < 4; i++)
    {
    	p += (boneWeights[i] * mul(position, boneTransforms[boneIndices[i]]));
    }
	p.w = 1.0f;
    return mul(p, mul(view,projection));
}