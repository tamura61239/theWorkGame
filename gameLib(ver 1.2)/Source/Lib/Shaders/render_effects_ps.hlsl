#include"render_effects.hlsli"
Texture2D color_map : register(t0);
Texture2D depth_map : register(t1);
Texture2D shadow_map : register(t2);
SamplerState color_map_sampler_state : register(s0);
SamplerComparisonState shadow_map_sampler_state : register(s1);
/*****************************************************/
//　　　　	シーンに影を付ける
/*****************************************************/

float4 main(VS_OUT pin, uint sample_index: SV_SAMPLEINDEX) : SV_TARGET
{

	float4 colorMapColour = color_map.Sample(color_map_sampler_state,pin.texcoord);
	float4 depthMapColour = depth_map.Sample(color_map_sampler_state, pin.texcoord);

	float3 fragmentColour = colorMapColour.rgb;
	float alpha = colorMapColour.a;

	float4 ndcPosition;
	ndcPosition.x = pin.texcoord.x * 2 - 1;
	ndcPosition.y = pin.texcoord.y * -2 + 1;
	ndcPosition.z = depthMapColour.x;
	ndcPosition.w = 1;

	float4 worldPosition = mul(ndcPosition, inverseViewProjection);
	worldPosition /= worldPosition.w;

	float4 lightViewPosition = mul(worldPosition, lightViewProjection);
	lightViewPosition /= lightViewPosition.w;
#if 1
	if (lightViewPosition.z < 1 && lightViewPosition.z>0)
	{
        float maxDepthSlope = max(abs(ddx(lightViewPosition.z)), abs(ddy(lightViewPosition.z)));

        float shadowBias2 = data.shadowBisa + data.slopeScaledBias * maxDepthSlope;
        shadowBias2 = min(shadowBias2, data.depthBiasClamp);


		lightViewPosition.x = lightViewPosition.x * 0.5 + 0.5;
		lightViewPosition.y = lightViewPosition.y * -0.5 + 0.5;
        float shadowThreshold = shadow_map.SampleCmpLevelZero(shadow_map_sampler_state, lightViewPosition.xy, lightViewPosition.z - shadowBias2);
		
		shadowThreshold = lerp(data.shadowColor, float3(1.0f, 1.0f, 1.0f), shadowThreshold).x;
		
		fragmentColour *= shadowThreshold;
	}
#else
//step(a,x):(x >= a) ? 1 : 0 (xがa以上の場合は1, それ以外は0を返します)
	lightViewPosition.x = lightViewPosition.x * 0.5 + 0.5;
	lightViewPosition.y = lightViewPosition.y * -0.5 + 0.5;

	float shadowThreshold = lerp(1.0f, shadow_map.SampleCmpLevelZero(shadow_map_sampler_state, lightViewPosition.xy, lightViewPosition.z - shadowBisa), step(lightViewPosition.z, 1) * step(0, lightViewPosition.z));
	shadowThreshold = lerp(shadowColor, float3(1.0f, 1.0f, 1.0f), shadowThreshold).x;
	fragmentColour *= shadowThreshold;
#endif
	//float shadowThreshold = lerp(1.0f, shadow_map.SampleCmpLevelZero(shadow_map_sampler_state, lightViewPosition.xy, lightViewPosition.z - shadowBisa), step(1, lightViewPosition.z) * (1 - step(lightViewPosition.z, 0)));
	//shadowThreshold = lerp(shadowColor, float3(1.0f, 1.0f, 1.0f), shadowThreshold).x;
	//fragmentColour *= shadowThreshold;
	return float4(fragmentColour, alpha);
}