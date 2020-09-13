#include "sprite.hlsli"   
#include "motionblur.hlsli"   
Texture2D diffuse_map : register(t0);
Texture2D velocity_map : register(t1);
SamplerState diffuse_map_sampler_state : register(s0);
float4 main(VS_OUT pin) : SV_TARGET
{
	//float4 color = diffuse_map.Sample(diffuse_map_sampler_state,pin.texcoord);
	//float2 velocity = velocity_map.Sample(diffuse_map_sampler_state, pin.texcoord).xy;

	//float blurSample = 16;

	//for (int i = 0; i < blurSample; i++)
	//{
	//	float t = i / blurSample;
	//	color += diffuse_map.Sample(diffuse_map_sampler_state, pin.texcoord + velocity * t);
	//}
	////color /= blurSample;
	//return color;
	int Blur = 10;

	float4 velocity = velocity_map.Sample(diffuse_map_sampler_state, pin.texcoord);
	velocity.xy /= (float)Blur;
	float4 color = diffuse_map.Sample(diffuse_map_sampler_state, pin.texcoord);
	int cnt = 1;
	float4 bColor = (float4)0;
	for (int i = cnt; i < Blur; i++)
	{
		bColor = diffuse_map.Sample(diffuse_map_sampler_state, pin.texcoord + velocity.xy * (float)i);
		if (velocity.a < bColor.a + 0.1f)
		{
			cnt++;
			color += bColor;
		}
	}
	color /= (float)cnt;
	return color;
	//float4 color = diffuse_map.Sample(diffuse_map_sampler_state, pin.texcoord);
	//color += velocity_map.Sample(diffuse_map_sampler_state, pin.texcoord) * 0.5f;
	//return color;
}