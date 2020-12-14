#include "model_motion_data.hlsli"

float4 main(GS_OUT gout) : SV_TARGET
{
    return float4(gout.velocity.x, -gout.velocity.y, gout.velocity.z, 1.0f);
}