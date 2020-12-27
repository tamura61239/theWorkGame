#include"particle_render.hlsli"

Texture2D diffuse_map : register(t0);
SamplerState diffuse_map_sampler_state : register(s0);
float4 main(GS_OUT pin) : SV_TARGET
{
    float4 color = diffuse_map.Sample(diffuse_map_sampler_state, pin.texcoord) * pin.color;
    if (color.a <= 0.f)
    {
        discard;
    }

	return color;
}