#include"deferred_stage_scene_particle.hlsli"

PS_OUT main(GS_OUT pin)
{
	PS_OUT pout;
    pout.color = pin.color;
	pout.velocity = float4(pin.velocity, 1);
	return pout;
}