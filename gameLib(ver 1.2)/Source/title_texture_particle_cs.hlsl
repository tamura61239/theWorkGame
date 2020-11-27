#include"title_texture_particle.hlsli"
#include"render_particle_cube.hlsli"

[numthreads(100, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	uint index = DTid.x + DTid.y + DTid.z;
	Particle p = particleBuffer[index];
	p.position += p.velocity*p.speed *elapsdTime;
	particleBuffer[index] = p;
	//描画用データの初期化
	ParticleRender render = (ParticleRender)0;
	//描画用データのセット
	render.position = float4(p.position,1);
	render.scale = scale;
	render.color = p.color;
	render.velocity = p.velocity;
	uint bufferIndex = index * 17 * 4;
	//描画用データの書き出し
	WriteRender(render, bufferIndex);

}