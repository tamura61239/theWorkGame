#include"title_psrticle_compute.hlsli"
#include"render_particle_cube.hlsli"
/****************************************************************************/
//　　　パーティクルが中央へ吸い込まれていく処理
/****************************************************************************/

[numthreads(100, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	//index値の取得
	uint index = DTid.x + DTid.y + DTid.z;
	uint bufferIndex = index * MAX;
	Particle p;
	//更新データの読み込み
	ReadParticle(p, bufferIndex);
    //更新
	float3 vec = float3(0, 0, 0) - p.position;
	float l = length(vec);
	vec = normalize(vec);
	float3 vec2 = cross(float3(0, 0, -1), vec);
	
	p.velocity = lerp((vec2 + vec) * p.speed * 10.f * elapsdTime, (vec2 + vec) * p.speed * l * elapsdTime, step(l,10));
	p.position += p.velocity;
	l = length(float3(0, 0, 0) - p.position);
	if (abs(l) <= 2.f)
	{//一定以下の距離になったら消す
		p.color = float4(0, 0, 0, 0);
		p.position = float3(0, 0, 0);
		p.scale = float3(0, 0, 0);
		p.life = 0;
	}
	//更新データの書き出し
	WriteParticle(p, bufferIndex);

	//描画用データの初期化
	ParticleRender render = (ParticleRender)0;
	//描画用データのセット
	render.position = float4(p.position, 1.0f);
	render.scale = p.scale;
	render.color = p.color;
	render.angle = p.angle;
	render.velocity = p.velocity;
	bufferIndex = index * 17 * 4;
	//描画用データの書き出し
	WriteRender(render, bufferIndex);

}