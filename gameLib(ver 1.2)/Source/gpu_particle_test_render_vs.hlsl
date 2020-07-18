#include"gpu_particle_test_render.hlsli"
VS_OUT main( float4 position : POSITION, float3 velocity : VELOCITY, float4 color : COLOR)
{
	VS_OUT vout;
	vout.position = mul(position, view);
	vout.position = mul(vout.position, projection);
	vout.color = color;
	return vout;
}