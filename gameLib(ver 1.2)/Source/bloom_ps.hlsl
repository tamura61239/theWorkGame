#include"bloom.hlsli"

Texture2D deffault_map:register(t0);
SamplerState deffault_map_sampler:register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
	uint mip_level = 0, width, height, number_of_levels;
	deffault_map.GetDimensions(mip_level, width, height, number_of_levels);

	const float offset[3] = { 0.0, 1.3846153846, 3.2307692308 };
	const float weight[3] = { 0.2270270270, 0.3162162162, 0.0702702703 };

	float4 color = deffault_map.Sample(deffault_map_sampler,pin.texcoord);

	for (int i = 0;i < blurCount;i++)
	{
		color += deffault_map.Sample(deffault_map_sampler, pin.texcoord + float2(offset[i] * widthBlur / width, offset[i] * hightBlur / height)) * weight[i];
		color += deffault_map.Sample(deffault_map_sampler, pin.texcoord - float2(offset[i] * widthBlur / width, offset[i] * hightBlur / height)) * weight[i];

	}

	return color;
}