#include "model_blur.hlsli"

VS_OUT main(
	float4 position     : POSITION,
	float3 normal : NORMAL,
	float2 texcoord : TEXCOORD,
	float4 boneWeights : WEIGHTS,
	uint4  boneIndices : BONES
)
{
	float3 p = { 0, 0, 0 };
	float3 bP = { 0, 0, 0 };
	float3 n = { 0, 0, 0 };
	for (int i = 0; i < 4; i++)
	{
		p += (boneWeights[i] * mul(position, boneTransforms[boneIndices[i]])).xyz;
		bP += (boneWeights[i] * mul(position, beforeBoneTransforms[boneIndices[i]])).xyz;
		n += (boneWeights[i] * mul(float4(normal.xyz, 1), boneTransforms[boneIndices[i]])).xyz;
	}
	//法線
	float3 N = normalize(mul(n, (float3x3)viewProjection));
	//今のフレームの座標
	float4 nowPosition = mul(float4(p, 1), viewProjection);
	//前のフレームの座標
	float4 beforePosition = mul(float4(bP, 1), beforeView);
	beforePosition = mul(beforePosition, beforeProjection);
	//移動ベクトル
	float3 vec = nowPosition.xyz - beforePosition.xyz;

	VS_OUT vout;

	float d = dot(normalize(vec), normalize(N));
	if (d < 0.0f)vout.position = beforePosition;
	else vout.position = nowPosition;
	vout.velocity.xy = ((nowPosition.xy / nowPosition.z) - (beforePosition.xy / beforePosition.z)) * 0.5f;
	vout.velocity.y *= -1.0f;
	vout.velocity.z = vout.position.z;
	vout.velocity.w = vout.position.w;
	return vout;
}
