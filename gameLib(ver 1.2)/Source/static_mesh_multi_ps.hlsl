#include"static_mesh_multi.hlsli"

Texture2D diffuse_map : register(t0);
SamplerState diffuse_map_sampler_state : register(s0);

PS_OUT main(VS_OUT pin)
{
	float4 color = diffuse_map.Sample(diffuse_map_sampler_state, pin.texcoord)*pin.color;
	PS_OUT pout;
	pout.color = color;
	pout.position = pin.worldPosition;
	pout.normal = float4(pin.worldNormal, 1);
	return pout;
}