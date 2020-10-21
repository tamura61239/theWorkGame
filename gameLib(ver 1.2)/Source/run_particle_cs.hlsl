#include"run_particle_cs_function.hlsli"
#include"render_particle_cube.hlsli"

[numthreads(100, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	uint index = DTid.x;
	uint bufferIndex = index * MAX;
	Particle p;
	ReadParticle(p, bufferIndex);

	p.position += p.velocity * elapsdTime;
	//p.scale += scaleAmount * elapsdTime;
	p.life -= p.lifeAmount * elapsdTime;
	p.color.a = p.life;
	if (p.life <= 0)
	{
		p.position = (float3)0;
		p.scale = 0;
		p.velocity = (float3)0;
		p.life = 0;
	}

	WriteParticle(p, bufferIndex);
	//描画用データの初期化
	ParticleRender render = (ParticleRender)0;
	//描画用データのセット
	render.position = float4(p.position, 1.0f);
	render.scale = float3(p.scale, p.scale, p.scale);
	render.color = p.color;
	render.angle = float3(0,0,0);
	render.velocity = p.velocity;
	bufferIndex = index * 17 * 4;
	//描画用データの書き出し
	WriteRender(render, bufferIndex);

}