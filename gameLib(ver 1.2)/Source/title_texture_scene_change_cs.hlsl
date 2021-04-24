#include"title_texture_particle.hlsli"
#include"render_particle_cube.hlsli"
#include"Lib/Shaders/rand_function.hlsli"
/****************************************************************************/
//　　　パーティクルが中央へ吸い込まれていく処理
/****************************************************************************/

[numthreads(100, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	//index値の取得
	uint index = DTid.x + DTid.y + DTid.z;
	//更新データの読み込み

	Particle p = particleBuffer[index];
	float z = p.position.z;
	float3 vec = float3(0, 0, 0) - p.position;
	float l = length(vec);
	p.speed = rand_1_normal(float2(DTid.x % 666, DTid.x % 294), 1) + 0.2f;
	p.speed *= (-l + 200) * 1.25f;
	vec = normalize(vec);
	float3 u = float3(0, 0, -1);
	float3 vec2 = cross(u, vec);
	z = 0 - z;
	p.velocity = (vec2 + vec / 5) * (p.speed + speed) * l * elapsdTime;
	p.velocity /= l;
	p.position += p.velocity;
	//更新データの書き出し
	particleBuffer[index] = p;
	
	ParticleRender render = (ParticleRender)0;
    //描画用データの更新
	render.position = float4(p.position, p.color.a);
	render.scale = scale;
	render.color = p.color;
	render.velocity = p.velocity;
	uint bufferIndex = index * 17 * 4;
	WriteRender(render, bufferIndex);

}