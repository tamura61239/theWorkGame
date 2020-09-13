#include"stage_scene_particle_blur.hlsli"

float4 main(GS_OUT pin) : SV_TARGET
{
	float4 velocity;
    velocity.xy = pin.velocity.xy;
    velocity.z = 1.0f;
    velocity.w = pin.velocity.z / pin.velocity.w;
    return velocity;
}