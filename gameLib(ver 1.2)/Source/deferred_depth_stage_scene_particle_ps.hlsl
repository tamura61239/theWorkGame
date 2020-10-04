#include"stage_obj_particle.hlsli"
#include"depth_of_field_data.hlsli"

#if DEFERRED

PS_DEPTH_OUT main(GS_OUT pin)
{
	PS_DEPTH_OUT pout;
	pout.color = pin.color;
	float z = pin.position.z / pin.position.w;
	//z *= 10;
	if (z <= 0.25f)pout.z.x = 4 * z;
	else if (z >= 0.75f)pout.z.x = 4 * (1 - z);
	else pout.z.x = 1;
	pout.z = float4(pout.z.x, pout.z.x, pout.z.x, 1);
	return pout;
}
#else
float4 main(GS_OUT pin):SV_TARGET
{
	float4 a = (float4)0;
	float z = pin.position.z / pin.position.w;
	if (z <= 0.25f)a = 4 * z;
	else if (z >= 0.75f)a = 4 * (1 - z);
	else a = 1;

	return a;
}
#endif