#include"bloom.hlsli"

Texture2D bloom_map_01:register(t0);
Texture2D bloom_map_02:register(t1);
Texture2D bloom_map_03:register(t2);
Texture2D bloom_map_04:register(t3);
SamplerState deffault_map_sampler:register(s2);

float4 main(VS_OUT pin) : SV_TARGET
{
	float4 color = (float4)0;
	color += bloom_map_01.Sample(deffault_map_sampler, pin.texcoord);
	color += bloom_map_02.Sample(deffault_map_sampler, pin.texcoord);
	color += bloom_map_03.Sample(deffault_map_sampler, pin.texcoord);
	color += bloom_map_04.Sample(deffault_map_sampler, pin.texcoord);
	color.a = 1.0f;
	return color;
}