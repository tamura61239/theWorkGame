#include"static_mesh_motion_data.hlsli"
#include"before_camera_view_data.hlsli"
#include"velocity_map_parameter.hlsli"
/*****************************************************************/
//　　　前のフレームと今のフレームのワールド座標を計算する
/*****************************************************************/

VS_OUT main(float4 position : POSITION, float3 normal : NORMAL, float2 texcoord : TEXCOORD)
{
    VS_OUT vout = (VS_OUT) 0;
    //今のフレームの座標
    float4 nowPosition, beforePosition;
    nowPosition = mul(position, world);
    nowPosition = mul(nowPosition, view);
    nowPosition = mul(nowPosition, projection);
    
    //前のフレームの座標
    beforePosition = mul(position, beforeWorld);
    beforePosition = mul(beforePosition, beforeView);
    beforePosition = mul(beforePosition, beforeProjection);
    
    float4 vec = (beforePosition - nowPosition) * 0.5f * value;
    vout.position = nowPosition;
    vout.beforePosition = nowPosition + vec;
    return vout;
}