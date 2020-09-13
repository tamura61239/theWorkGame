#include "static_mesh_blur.hlsli"

VS_OUT main(float4 position : POSITION, float3 normal : NORMAL, float2 texcoord : TEXCOORD/*UNIT.13*/)
{
	VS_OUT vout;
	//今のフレームの座標
	float4 nowPosition = mul(position, world);
	nowPosition = mul(nowPosition, view);
	nowPosition = mul(nowPosition, projection);
	//前のフレームの座標
	float4 beforePosition = mul(position, beforeWorld);
	beforePosition = mul(beforePosition, beforeView);
	beforePosition = mul(beforePosition, beforeProjection);
	//法線
	float3 N = normalize(mul(normal, /*(float3x3)world*/mul((float3x3)world, (float3x3)mul(view, projection))));
	//移動ベクトル
	float3 vec = nowPosition.xyz - beforePosition.xyz;
	//前のフレームと今のフレームの内積
	float d = dot(normalize(vec), normalize(N));
	if (d < 0.0f)vout.position = beforePosition;
	else vout.position = nowPosition;
	vout.velocity.xy = ((nowPosition.xy/ nowPosition.z) - (beforePosition.xy/ beforePosition.z))*0.5f;
	vout.velocity.y *= -1.0f;
	vout.velocity.z = vout.position.z;
	vout.velocity.w = vout.position.w;
	return vout;
}
