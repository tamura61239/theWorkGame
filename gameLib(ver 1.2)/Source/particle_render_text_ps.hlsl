#include"particle_render.hlsli"

Texture2D diffuse_map : register(t0);
SamplerState diffuse_map_sampler_state : register(s0);
float4 main(GS_OUT2 pin) : SV_TARGET
{
	return diffuse_map.Sample(diffuse_map_sampler_state, pin.texcoord) * pin.color;
}