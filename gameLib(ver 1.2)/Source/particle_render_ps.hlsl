#include"particle_render.hlsli"
/*****************************************************************/
//　　　ピクセル単位で色を付ける
/*****************************************************************/

float4 main(GS_OUT pin) : SV_TARGET
{
    if (pin.color.a <= 0.f)
    {
        discard;
    }

	return pin.color;
}