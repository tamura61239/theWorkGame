#include"bloom.hlsli"

Texture2D deffault_map:register(t0);
SamplerState deffault_map_sampler:register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
	uint mip_level = 0, width, height, number_of_levels;
	deffault_map.GetDimensions(mip_level, width, height, number_of_levels);


	float4 color = (float4)0;

	for (int i = 1;i < blurCount;i++)
	{
		//color += deffault_map.Sample(deffault_map_sampler, pin.texcoord + float2(offset[i] * widthBlur / width, offset[i] * hightBlur / height)) * weight[i];
		//color += deffault_map.Sample(deffault_map_sampler, pin.texcoord + float2(offset[i] * widthBlur / width, 0)) * weight[i];
		//color += deffault_map.Sample(deffault_map_sampler, pin.texcoord - float2(offset[i] * widthBlur / width, offset[i] * hightBlur / height)) * weight[i];
		//color += deffault_map.Sample(deffault_map_sampler, pin.texcoord - float2(offset[i] * widthBlur / width, 0)) * weight[i];
		//color += offset[i].z * deffault_map.Sample(deffault_map_sampler, pin.texcoord + offset[i].xy);
		//color += offset[i].z * deffault_map.Sample(deffault_map_sampler, pin.texcoord - offset[i].xy);
		color += offset[i].z * deffault_map.Sample(deffault_map_sampler, pin.texcoord + float2(offset[i].x, 0));
		color += offset[i].z * deffault_map.Sample(deffault_map_sampler, pin.texcoord + float2(0, offset[i].y));
		color += offset[i].z * deffault_map.Sample(deffault_map_sampler, pin.texcoord - float2(offset[i].x, 0));
		color += offset[i].z * deffault_map.Sample(deffault_map_sampler, pin.texcoord - float2(0, offset[i].y));

	}
	color.w = 1.0f;
	return color;
}