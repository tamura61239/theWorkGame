#include"title_psrticle_compute.hlsli"
#include"render_particle_cube.hlsli"
#include"curl_noise.hlsli"
#include"matrix_calculation.hlsli"
/****************************************************************************/
//　　　パーティクルを更新
/****************************************************************************/

[numthreads(100, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	//index値の取得
	uint index = DTid.x + DTid.y + DTid.z;
	uint bufferIndex = index * MAX;
	Particle p;
	//更新データの読み込み
	ReadParticle(p, bufferIndex);

	//更新
	p.moveAngle += p.moveAngleMovement * elapsdTime;

	p.defPosition += p.defVelocity * p.speed * elapsdTime;
	p.life -= elapsdTime;

	float3 right = cross(normalize(p.defVelocity), float3(1, 1, 0));

	p.moveAngle += p.moveAngleMovement * elapsdTime;

	p.position = p.defPosition + right * sin(p.moveAngle) * p.moveAngleLength;

	p.life -= elapsdTime;
	if (p.life <= 0)
	{//寿命が尽きたら消す
		p.color = float4(0, 0, 0, 0);
		p.position = float3(0, 0, 0);
		p.scale = float3(0, 0, 0);
	}
	//更新データの書き出し
	WriteParticle(p, bufferIndex);

	ParticleRender render = (ParticleRender)0;
    //描画用データの更新
	render.position = float4(p.position, 1.0f);
	render.scale = p.scale;
	render.color = p.color;
	render.angle = p.angle;
	render.velocity = p.velocity;
	bufferIndex = index * 17 * 4;
	WriteRender(render, bufferIndex);
}