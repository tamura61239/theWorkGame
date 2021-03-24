#include "model_motion_data.hlsli"
#include"before_camera_view_data.hlsli"
#include"velocity_map_parameter.hlsli"


VS_OUT main(
	float4 position : POSITION,
	float3 normal : NORMAL,
	float2 texcoord : TEXCOORD,
	float4 boneWeights : WEIGHTS,
	uint4 boneIndices : BONES
)
{
    float3 p0 = { 0, 0, 0 };
    float3 p1 = { 0, 0, 0 };
	
	[unroll]
    for (int i = 0; i < 4; i++)
    {
        p0 += (boneWeights[i] * mul(position, boneTransforms[boneIndices[i]])).xyz;
        p1 += (boneWeights[i] * mul(position, beforeBoneTransforms[boneIndices[i]])).xyz;
    }    
	VS_OUT vout = (VS_OUT) 0;

    float4 nowPosition, beforePosition;
    //今のフレームの座標
    nowPosition = mul(float4(p0, 1), viewProjection);
    //前のフレームの座標
    beforePosition = mul(float4(p1, 1), mul(beforeView,beforeProjection));

    float4 vec = (beforePosition - nowPosition) * 0.5f * value;
    vout.position = nowPosition;
    vout.beforePosition = nowPosition + vec;
    
	return vout;
}