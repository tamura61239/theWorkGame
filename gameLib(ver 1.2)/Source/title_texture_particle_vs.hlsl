#include"stage_obj_particle.hlsli"
VS_OUT main(float3 position : POSITION, float3 velocity : VELOCITY,float speed:SPEED, float4 color : COLOR, float life : LIFE)
{
	VS_OUT vout;
	vout.position = float4(position,1);
	vout.scale = float3(2,2,2);
	vout.rotateMatrix = mul(MakeRotation(float3(1, 0, 0), 0), mul(MakeRotation(float3(0, 1, 0), 0), MakeRotation(float3(0, 0, 1), 0)));
	vout.color = color;
	return vout;
}