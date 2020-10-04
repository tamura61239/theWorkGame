#include"title_psrticle_compute.hlsli"
#include"render_particle_cube.hlsli"
[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	//float number = 0;
	//for (int i = 0; i < 100000; i++)
	//{
	//	uint bufferIndex = 29 * i;
	//	Particle p;
	//	ReadParticle(p, bufferIndex);
	//	if (p.life <= 0)continue;
	//	ParticleRender render;
	//	render.position = p.position;
	//	render.angle = p.angle;
	//	render.color = p.color;
	//	render.velocity = p.velocity;
	//	render.scale = float3(0.2, 0.2, 0.2);
	//	float index = number * 17 * 4;
	//	WriteRender(render, index);
	//	number++;
	//}
	//numberBuffer.Store(0, asuint(number));
}