#include"fireworks_particle.hlsli"
#include"render_particle_cube.hlsli"

[numthreads(100, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	uint index = DTid.x + DTid.y + DTid.z;
	Particle p = particleBuffer[index];
    float3 velocity = p.velocity;
    p.velocity += p.accel * pow(1 - p.life * (p.endTimer - p.endTime) / p.endTimer, 2) * elapsdTime;
    p.velocity.xz = lerp(velocity.xz, p.velocity.xz, step(abs(p.velocity.xz-velocity.xz), abs(p.velocity.xz - 0)));
	p.position += p.velocity * elapsdTime;
	p.life -= p.lifeAmoust*elapsdTime;
	p.endTime += step(p.life, 0) * elapsdTime;
	//float4 color = lerp(p.startColor, p.endColor, step(p.life, 0) * (p.endTime / p.endTimer));
	float4 color = p.startColor;
    color.a = lerp(1, 0, step(p.life, 0) * (p.endTime / p.endTimer));
	if (p.endTime >= p.endTimer)
	{
		color = float4(0, 0, 0, 0);
		p.position = float3(0, 0, 0);
		p.velocity = float3(0, 0, 0);
		p.life = -1;
		p.scale = 0;
		p.endTime = p.endTimer;
	}
	particleBuffer[index] = p;
	//描画用データの初期化
	ParticleRender render = (ParticleRender)0;
	//描画用データのセット
	render.position = float4(p.position, color.a);
	render.scale = p.scale;
	render.color = color;
	render.velocity = p.velocity;
	uint bufferIndex = index * 17 * 4;
	//描画用データの書き出し
	WriteRender(render, bufferIndex);

}