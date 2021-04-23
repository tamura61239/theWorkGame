#include"particle_motiom_data_render.hlsli"
#include"matrix_calculation.hlsli"
/*****************************************************************/
//　　　ワールド座標系の点からスクリーン座標系に変換したりする
/*****************************************************************/

VS_OUT main(float4 position : POSITION, float3 angle : ANGLE, float4 color : COLOR, float3 velocity : VELOCITY, float3 scale : SCALE)
{
    VS_OUT vout;
    vout.worldPosition = position.xyz;
    vout.position = mul(float4(position.xyz, 1), view);
    vout.position = mul(vout.position, projection);
    vout.scale = scale;
    vout.rotateMatrix = mul(MakeRotation(float3(1, 0, 0), angle.x), mul(MakeRotation(float3(0, 1, 0), angle.y), MakeRotation(float3(0, 0, 1), angle.z)));
    vout.color = color;
    vout.velocity = velocity;
    return vout;
}