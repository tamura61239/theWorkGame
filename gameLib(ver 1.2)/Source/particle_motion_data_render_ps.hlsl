#include"particle_motiom_data_render.hlsli"
#define BLUER_TYPE 0
//PS_OUT main(GS_OUT pin)
//{
//    PS_OUT pout = (PS_OUT) 0;
//    pout.color = pin.color;
//#if BLUER_TYPE
//    pout.velocity = float4(pin.velocity.x,pin.velocity.y,pin.velocity.z, 1);
//#else
//    pout.velocity = float4(pin.velocity.x,-pin.velocity.y,pin.velocity.z, 1);
//#endif
//    return pout;
//}
float4 main(GS_OUT pin):SV_TARGET
{
    return float4(pin.color.x, -pin.color.y, pin.color.z, 1);
}