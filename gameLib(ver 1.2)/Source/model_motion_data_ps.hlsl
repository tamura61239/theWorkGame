#include "model_motion_data.hlsli"
/*****************************************************************/
//�@�@�@�擾�����ړ��x�N�g�������ɑ��x�}�b�v�̐F�����߂�
/*****************************************************************/

float4 main(GS_OUT gout) : SV_TARGET
{
    return float4(gout.velocity.x, -gout.velocity.y, gout.velocity.z, 1.0f);
}