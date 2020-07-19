#include"bloom.hlsli"

Texture2D deffault_map:register(t0);
SamplerState deffault_map_sampler:register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
	float4 color = deffault_map.Sample(deffault_map_sampler,pin.texcoord);
	float luminance = dot(color.rgb, float3(0.2126, 0.7152, 0.0722));

	//float3 c = color.rgb - float3(0.4f, 0.4f, 0.4f);
	//c = max(c, 0);
	//color.rgb *= 1-step(c, 0);
	//color.rgb *= 0.5f;
	color.rgb = lerp(float3(0, 0, 0),color.rgb , step(luminance, threshold));
	return color;
}