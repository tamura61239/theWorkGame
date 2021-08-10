#include"particle_motiom_data_render.hlsli"
/*****************************************************************/
//　　　テクスチャから色を取得する
/*****************************************************************/

Texture2D diffuse_map : register(t0);
SamplerState diffuse_map_sampler_state : register(s0);

float4 main(GS_OUT pin) : SV_TARGET
{
    return diffuse_map.Sample(diffuse_map_sampler_state, pin.texcoord).a * float4(pin.color.x, -pin.color.y, pin.color.z, 1);
}