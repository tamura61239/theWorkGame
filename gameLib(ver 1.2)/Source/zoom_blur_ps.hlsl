#include "sprite.hlsli"   
#include"rand_function.hlsli"
Texture2D diffuse_map : register(t0);
SamplerState diffuse_map_sampler_state : register(s0);
float4 main(VS_OUT pin) : SV_TARGET
{
	float2 center = float2(0.5,0.5);
	float2 vec = pin.texcoord - center;
	float4 color = (float4)0;
	float rand = rand_1_normal(vec, 0.5f);
	float total = 0;
	float nFrag = 1.0f / 20.0f;
	for (int i = 0; i < 20; i++)
	{
		float percent = (i + rand) * nFrag;
		float weight = percent - percent * percent;

		float2 uv = pin.texcoord - vec *percent * 3.0f * nFrag;
		total += weight;
		color += diffuse_map.Sample(diffuse_map_sampler_state, uv) * pin.color * weight;
	}
	color.w = 1.0f;
	color.rgb /= total;
	return color;
}