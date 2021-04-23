#include"bloom.hlsli"

Texture2D deffault_map:register(t0);
SamplerState deffault_map_sampler:register(s2);
/*****************************************************/
//　　　ブルームに使うテクスチャをぼかす処理①
/*****************************************************/

float4 main(VS_OUT pin) : SV_TARGET
{
	//テクスチャからデータを取得
	uint mip_level = 0, width, height, number_of_levels;
	deffault_map.GetDimensions(mip_level, width, height, number_of_levels);

	//uvをずらしながら色を取得する
	float4 color = (float4)0;
	[unroll]
	for (int i = 1;i < blurCount;i++)
	{
		color += offset[i].z * deffault_map.Sample(deffault_map_sampler, pin.texcoord + float2(offset[i].x, 0));
		color += offset[i].z * deffault_map.Sample(deffault_map_sampler, pin.texcoord + float2(0, offset[i].y));
		color += offset[i].z * deffault_map.Sample(deffault_map_sampler, pin.texcoord - float2(offset[i].x, 0));
		color += offset[i].z * deffault_map.Sample(deffault_map_sampler, pin.texcoord - float2(0, offset[i].y));

	}
	//アルファー値を1にする
	color.w = 1.0f;
	return color;
}