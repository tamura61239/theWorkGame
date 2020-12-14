#include"particle_motiom_data_render.hlsli"
float4 main(GS_OUT pin) : SV_TARGET
{
    return pin.color;
}