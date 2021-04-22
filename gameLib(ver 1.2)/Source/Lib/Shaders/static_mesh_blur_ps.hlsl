#include "static_mesh_blur.hlsli"

float4 main(VS_OUT pin) : SV_TARGET
{
	float4 vec = (float4)0;
	vec.xy = pin.velocity.xy;
	vec.z = 1;
	vec.w = pin.velocity.z / pin.velocity.w;
	return vec;
}