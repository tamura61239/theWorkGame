#include"particle_motiom_data_render.hlsli"
/*****************************************************************/
//　　　色を付ける
/*****************************************************************/

float4 main(GS_OUT pin) : SV_TARGET
{
    return pin.color;
}