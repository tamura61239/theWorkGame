#include"static_mesh_motion_data.hlsli"
#include"before_camera_view_data.hlsli"
#include"velocity_map_parameter.hlsli"
/*****************************************************************/
//�@�@�@�O�̃t���[���ƍ��̃t���[���̃��[���h���W���v�Z����
/*****************************************************************/

VS_OUT main(float4 position : POSITION, float3 normal : NORMAL, float2 texcoord : TEXCOORD)
{
    VS_OUT vout = (VS_OUT) 0;
    //���̃t���[���̍��W
    float4 nowPosition, beforePosition;
    nowPosition = mul(position, world);
    nowPosition = mul(nowPosition, view);
    nowPosition = mul(nowPosition, projection);
    
    //�O�̃t���[���̍��W
    beforePosition = mul(position, beforeWorld);
    beforePosition = mul(beforePosition, beforeView);
    beforePosition = mul(beforePosition, beforeProjection);
    
    float4 vec = (beforePosition - nowPosition) * 0.5f * value;
    vout.position = nowPosition;
    vout.beforePosition = nowPosition + vec;
    return vout;
}