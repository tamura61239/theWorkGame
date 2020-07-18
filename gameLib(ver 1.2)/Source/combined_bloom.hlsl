#include"bloom.hlsli"

Texture2D deffault_map:register(t0);
Texture2D bloom_map_01:register(t1);
Texture2D bloom_map_02:register(t2);
Texture2D bloom_map_03:register(t3);
Texture2D bloom_map_04:register(t4);
SamplerState deffault_map_sampler:register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
	float4 color = deffault_map.Sample(deffault_map_sampler,pin.texcoord);
	color += bloom_map_01.Sample(deffault_map_sampler, pin.texcoord);
	color += bloom_map_02.Sample(deffault_map_sampler, pin.texcoord);
	color += bloom_map_03.Sample(deffault_map_sampler, pin.texcoord);
	color += bloom_map_04.Sample(deffault_map_sampler, pin.texcoord);
	color.a = 1.0f;
	return color;
}