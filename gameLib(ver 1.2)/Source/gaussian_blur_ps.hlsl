#include "sprite.hlsli"   
Texture2D diffuse_map : register(t0);
SamplerState diffuse_map_sampler_state : register(s0);
float4 main(VS_OUT pin) : SV_TARGET
{
	uint mmipLevel = 0,width,height,mipNumber;
    diffuse_map.GetDimensions(mmipLevel, width, height, mipNumber);

	const float offset[3] = { 0.0, 1.3846153846, 3.2307692308 };
	const float weight[3] = { 0.2270270270, 0.3162162162, 0.0702702703 };

	float4 color = diffuse_map.Sample(diffuse_map_sampler_state, pin.texcoord) * weight[0];

	[loop]
	for (int i = 1; i < 3; i++)
	{
		color += diffuse_map.Sample(diffuse_map_sampler_state, pin.texcoord + float2(offset[i] / width, 0)) * weight[i];
		color += diffuse_map.Sample(diffuse_map_sampler_state, pin.texcoord - float2(offset[i] / width, 0)) * weight[i];
		color += diffuse_map.Sample(diffuse_map_sampler_state, pin.texcoord + float2(0, offset[i] / height)) * weight[i];
		color += diffuse_map.Sample(diffuse_map_sampler_state, pin.texcoord - float2(0, offset[i] / height)) * weight[i];

	}
	color /= 2;
	return color * pin.color;
}