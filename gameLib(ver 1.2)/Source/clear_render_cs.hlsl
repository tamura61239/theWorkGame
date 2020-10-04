#include"render_particle_cube.hlsli"

[numthreads(100, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	uint bufferIndex = DTid.x * 17 * 4;
	ParticleRender render;
	render.position = float4(0,0,0,0);
	render.angle = float3(0,0,0);
	render.color = float4(0, 0, 0, 0);
	render.velocity = float3(0, 0, 0);
	render.scale = float3(0, 0, 0);
	WriteRender(render, bufferIndex);

}