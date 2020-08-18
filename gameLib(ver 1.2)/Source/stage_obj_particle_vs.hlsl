#include"stage_obj_particle.hlsli"
VS_OUT main( float4 position : POSITION,float life:LIFE,float3 scale:SCALE,float4 color:COLOR,float3 velocity:VELOCITY,float colorType:COLORTYPE,float3 angle:ANGLE)
{
	VS_OUT vout;
	vout.position = position;
	vout.scale = scale;
	vout.rotateMatrix = mul(MakeRotation(float3(1, 0, 0), angle.x), mul(MakeRotation(float3(0, 1, 0), angle.y), MakeRotation(float3(0, 0, 1), angle.z)));
	vout.color = color;
	return vout;
}