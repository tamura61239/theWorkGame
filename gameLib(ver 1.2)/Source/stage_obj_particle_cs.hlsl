#include"stage_obj_particle_cs_function.hlsli"
#include"curl_noise.hlsli"
[numthreads(100, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	//今のスレッドのID
	uint index = DTid.x + DTid.y + DTid.z;
	//出たスレッドIDにバッファに格納されてる構造体のサイズ
	uint bufferIndex = index * 19 * FLOAT_SIZE;

	uint particleNumber = index % 1000;
	uint objNumber = index / 1000;

	Particle p;
	p.position = asfloat(rwBuffer.Load4(bufferIndex));
	p.life = asfloat(rwBuffer.Load(bufferIndex + 4 * FLOAT_SIZE));
	p.scale = asfloat(rwBuffer.Load3((bufferIndex + 5 * FLOAT_SIZE)));
	p.color = asfloat(rwBuffer.Load4((bufferIndex + 8 * FLOAT_SIZE)));
	p.velocity = asfloat(rwBuffer.Load3((bufferIndex + 12 * FLOAT_SIZE)));
	p.colorType = asfloat(rwBuffer.Load((bufferIndex + 15 * FLOAT_SIZE)));
	p.angle = asfloat(rwBuffer.Load3((bufferIndex + 16 * FLOAT_SIZE)));

	if (particleNumber >= startIndex && particleNumber < startIndex + indexSize)//初期化
	{
		if (p.life <= 0)p = Init(objNumber);
	}
	if (changeColorFlag >= 1)
	{
		p.colorType = 2;
	}
	//更新
	p.position.xyz += p.velocity * elapsdTime;
	p.life -= (elapsdTime / maxLife) * lerp(1, 3, step(2, p.colorType));
	p.angle += angleMovement * elapsdTime;
	p.scale += p.scale * lerp(0, 1.5f, step(2, p.colorType)) * elapsdTime;
	float end = step(0, p.life);
	p.color.w = lerp(0, 1, p.life) * end;
	//セット
	rwBuffer.Store4(bufferIndex, asuint(p.position * end));
	rwBuffer.Store(bufferIndex + 4 * FLOAT_SIZE, asuint(p.life));
	rwBuffer.Store3(bufferIndex + 5 * FLOAT_SIZE, asuint(p.scale * end));
	rwBuffer.Store4(bufferIndex + 8 * FLOAT_SIZE, asuint(p.color * end));
	rwBuffer.Store3(bufferIndex + 12 * FLOAT_SIZE, asuint(p.velocity * end));
	rwBuffer.Store(bufferIndex + 15 * FLOAT_SIZE, asuint(p.colorType));
	rwBuffer.Store3(bufferIndex + 16 * FLOAT_SIZE, asuint(p.angle * end));
}