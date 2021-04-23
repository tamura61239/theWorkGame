#include"static_mesh.hlsli"
/*****************************************************/
//　　　　	アルファー値が一定以上のの色だけ取得
/*****************************************************/

float4 main(VS_OUT pin) : SV_TARGET
{
    if (pin.color.a <= 0.3f)
    {
        discard;
    }
    return pin.color;
}