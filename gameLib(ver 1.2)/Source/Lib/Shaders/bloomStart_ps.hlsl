#include"bloom.hlsli"
/*****************************************************/
//　　　    一定以上の輝度を取得する
/*****************************************************/

Texture2D deffault_map:register(t0);
SamplerState deffault_map_sampler:register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
	float4 color = deffault_map.Sample(deffault_map_sampler,pin.texcoord);
	color = max(float4(0, 0, 0, 0), color - threshold);
	return color;
}