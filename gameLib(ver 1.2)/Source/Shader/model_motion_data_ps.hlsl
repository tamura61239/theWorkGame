#include "model_motion_data.hlsli"
/*****************************************************************/
//　　　取得した移動ベクトルを元に速度マップの色を決める
/*****************************************************************/

float4 main(GS_OUT gout) : SV_TARGET
{
    return float4(gout.velocity.x, -gout.velocity.y, gout.velocity.z, 1.0f);
}