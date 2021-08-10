#include"particle_motiom_data_render.hlsli"
#define BLUER_TYPE 0
/*****************************************************************/
//　　　色を付ける
/*****************************************************************/
float4 main(GS_OUT pin):SV_TARGET
{
    return float4(pin.color.x, -pin.color.y, pin.color.z, 1);
}