#include "sprite.hlsli"   
Texture2D diffuse_map : register(t0);
Texture2D depth_map : register(t1);
Texture2D diffuse1_map : register(t2);
Texture2D diffuse2_map : register(t3);
SamplerState diffuse_map_sampler_state : register(s0);
float4 main(VS_OUT pin) : SV_TARGET
{
	float4 color0 = diffuse_map.Sample(diffuse_map_sampler_state, pin.texcoord);
	float4 color1 = diffuse1_map.Sample(diffuse_map_sampler_state, pin.texcoord);
	float4 color2 = diffuse2_map.Sample(diffuse_map_sampler_state, pin.texcoord);
	float4 color = (float4)0;
	float depth = depth_map.Sample(diffuse_map_sampler_state, pin.texcoord).r;

	float a = 1 / 0.35f;
	float b = 1 / 0.65f;
	if (depth >= 1.0f)
	{
		color = color0;
	}
	else if (depth >= 0.65f)
	{
		color = lerp(color1, color0, (depth - 0.65f) * a);
	}
	else
	{
		color = lerp(color1, color2, (depth) * b);
	}
	return  color *pin.color;
}