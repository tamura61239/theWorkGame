#include"title_texture_particle.hlsli"
#include"render_particle_cube.hlsli"

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	uint index = DTid.x;
	uint bufferIndex = index * MAX;
	Particle p = (Particle)0;
	ReadParticle(p, bufferIndex);
	float3 vec = float3(0, 0, 0) - p.position;
	float l = length(vec);
	vec = normalize(vec);
	float3 vec2 = cross(float3(0, 0, -1), vec);

	p.velocity = lerp((vec2 + vec) *100 * elapsdTime, (vec2 + vec) * 10 * l * elapsdTime, step(l, 10));
	//p.position += p.velocity;
	//l = length(float3(0, 0, 0) - p.position);
	//if (abs(l) <= 2.f)
	//{
	//	p.color = float4(0, 0, 0, 0);
	//	p.position = float3(0, 0, 0);
	//}

	WriteParticle(p, bufferIndex);
	//描画用データの初期化
	ParticleRender render = (ParticleRender)0;
	//描画用データのセット
	render.position = float4(p.position, 1.0f);
	render.scale = float3(1,1,1)*2;
	render.color = p.color;
	render.angle =float3(0,0,0);
	render.velocity = float3(0,0,0);
	bufferIndex = index * 17 * 4;
	//描画用データの書き出し
	WriteRender(render, bufferIndex);

}