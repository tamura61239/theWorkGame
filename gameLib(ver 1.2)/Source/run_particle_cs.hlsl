#include"run_particle_cs_function.hlsli"
RWByteAddressBuffer rwBuffer:register(u0);
ByteAddressBuffer randBuffer:register(t0);
#define FLOAT_SIZE 4
[numthreads(100, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	uint index = DTid.x + DTid.y + DTid.z;

	uint bufferIndex = index * 21 * FLOAT_SIZE;
	//初期化
	Particle p;
	p.position = asfloat(rwBuffer.Load4(bufferIndex));
	p.scale = asfloat(rwBuffer.Load3(bufferIndex + FLOAT_SIZE * 4));
	p.angle = asfloat(rwBuffer.Load3(bufferIndex + FLOAT_SIZE * 7));
	p.color = asfloat(rwBuffer.Load4(bufferIndex + FLOAT_SIZE * 10));
	p.velocity = asfloat(rwBuffer.Load3(bufferIndex + FLOAT_SIZE * 14));
	p.accel = asfloat(rwBuffer.Load3(bufferIndex + FLOAT_SIZE * 17));
	p.life = asfloat(rwBuffer.Load(bufferIndex + FLOAT_SIZE * 20));
	if (createFlag == 1)
	{
		if (startIndex <= index && startIndex + 10 + 90 * moveType > index)
		{
			uint r1 = index % (uint)rand1;
			uint r2 = index % (uint)rand2;
			uint r = r1 + r2;
			r %= (uint)totalRand;
			r *= 2 * FLOAT_SIZE;
			float2 rand = asfloat(randBuffer.Load2(r));
			float z = rand.y;
			float x = sqrt(1 - z * z) * cos(rand.x);
			float y = sqrt(1 - z * z) * sin(rand.x);
			p.position = playerPosition;
			p.position.xyz += float3(x, y, z);
			p.velocity = float3(x * 5, y, z * 5);
			p.accel = lerp(-normalize(playerVelocity), normalize(p.velocity) * 5, moveType);
			p.angle = float3(0, rand.x, 0);
			p.color = color;
			p.color.w = 0;
			p.scale = float3(1, 1, 1) * .5f;
			p.life = maxLife / (1 + moveType);

			rwBuffer.Store4(bufferIndex, asuint(p.position));
			rwBuffer.Store3(bufferIndex + FLOAT_SIZE * 4, asuint(p.scale));
			rwBuffer.Store3(bufferIndex + FLOAT_SIZE * 7, asuint(p.angle));
			rwBuffer.Store4(bufferIndex + FLOAT_SIZE * 10, asuint(p.color));
			rwBuffer.Store3(bufferIndex + FLOAT_SIZE * 14, asuint(p.velocity));
			rwBuffer.Store3(bufferIndex + FLOAT_SIZE * 17, asuint(p.accel));
			rwBuffer.Store(bufferIndex + FLOAT_SIZE * 20, asuint(p.life));
			return;
		}
	}
	if (p.life <= 0)return;
	//更新
	p.velocity += p.accel * elapsdTime * 60;
	p.position.xyz += p.velocity * elapsdTime;
	p.angle += angleMovement * elapsdTime;
	p.life -= elapsdTime;
	//徐々に大きくする
	p.scale += lerp(float3(0.1, 0.1, 0.1), float3(0, 0, 0), step(1.5f, p.scale.x)) * elapsdTime * 60;
	//徐々に不透明にしていく
	p.color.w += lerp(0.1f, 0, step(color.w, p.color.w)) * elapsdTime * 60;
	//パーティクルの寿命が尽きたかどうか
	float end = step(0, p.life);
	rwBuffer.Store4(bufferIndex, asuint(p.position * end));
	rwBuffer.Store3(bufferIndex + FLOAT_SIZE * 4, asuint(p.scale * end));
	rwBuffer.Store3(bufferIndex + FLOAT_SIZE * 7, asuint(p.angle * end));
	rwBuffer.Store4(bufferIndex + FLOAT_SIZE * 10, asuint(p.color * end));
	rwBuffer.Store3(bufferIndex + FLOAT_SIZE * 14, asuint(p.velocity * end));
	rwBuffer.Store3(bufferIndex + FLOAT_SIZE * 17, asuint(p.accel * end));
	rwBuffer.Store(bufferIndex + FLOAT_SIZE * 20, asuint(p.life * end));

}