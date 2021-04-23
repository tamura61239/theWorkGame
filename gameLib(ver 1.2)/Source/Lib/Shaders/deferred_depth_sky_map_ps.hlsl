#include"sky_map.hlsli"
#include"depth_of_field_data.hlsli"
TextureCube cubemap : register(t0);
SamplerState anisotropic_sampler_state : register(s0);

/*****************************************************/
//　　　　マルチレンダーターゲット用
/*****************************************************/

PS_DEPTH_OUT main(VS_OUT pin)
{
	PS_DEPTH_OUT pout;
	pout.color = float4(cubemap.Sample(anisotropic_sampler_state,pin.texcoord).rgb * materialColor.rgb,materialColor.a);
	pout.z = pin.position.z;
	return pout;
}