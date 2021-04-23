#include"sky_map.hlsli"
TextureCube cubemap : register(t0);
SamplerState anisotropic_sampler_state : register(s0);

/*****************************************************/
//　　　　	座標に合わせた画像に色を取得
/*****************************************************/

float4 main(VS_OUT pin) : SV_TARGET
{
	float4 color = float4(cubemap.Sample(anisotropic_sampler_state,pin.texcoord).rgb * materialColor.rgb,materialColor.a);
	return color;
}