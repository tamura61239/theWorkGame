#include"gpu_particle_test_render.hlsli"

float4 main(VS_OUT pin) : SV_TARGET
{
	return pin.color;
}