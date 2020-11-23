#include"title_texture_particle.hlsli"
#include"render_particle_cube.hlsli"

[numthreads(100, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	uint index = DTid.x + DTid.y + DTid.z;

	Particle p = particleBuffer[index];
	float z = p.position.z;
	float3 vec = float3(0, 0, 0) - p.position;
	float l = length(vec);
	vec = normalize(vec);
	float3 u = float3(0, 0, -1);
	float3 vec2 = cross(u, vec);
	z = 0 - z;
	p.velocity = /*lerp((vec2 + vec) * (p.speed+speed) * 5 * elapsdTime, */(vec2 + vec/5) * (p.speed + speed) * l * elapsdTime/*, step(l, 5))*/;
	p.velocity /= l;
	p.position += p.velocity;
	particleBuffer[index] = p;
	//描画用データの初期化
	ParticleRender render = (ParticleRender)0;
	//描画用データのセット
	render.position = float4(p.position,p.color.a);
	render.scale = scale;
	render.color = p.color;
	render.velocity = p.velocity;
	uint bufferIndex = index * 17 * 4;
	//描画用データの書き出し
	WriteRender(render, bufferIndex);

}