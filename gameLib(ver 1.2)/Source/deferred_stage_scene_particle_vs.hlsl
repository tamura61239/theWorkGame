#include"deferred_stage_scene_particle.hlsli"
VS_OUT main(float4 position : POSITION, float life : LIFE, float3 scale : SCALE, float4 color : COLOR, float3 velocity : VELOCITY, float3 angle : ANGLE)
{
	VS_OUT vout;
	vout.position = position;
	vout.scale = scale;
	vout.angle = angle;
	vout.color = color;
	vout.velocity = velocity;
	return vout;
}