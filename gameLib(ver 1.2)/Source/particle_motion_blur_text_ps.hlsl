#include"particle_motiom_data_render.hlsli"
/*****************************************************************/
//�@�@�@�e�N�X�`������F���擾����
/*****************************************************************/

Texture2D diffuse_map : register(t0);
SamplerState diffuse_map_sampler_state : register(s0);
float4 main(GS_OUT pin) : SV_TARGET
{
    return diffuse_map.Sample(diffuse_map_sampler_state, pin.texcoord) * pin.color;
}