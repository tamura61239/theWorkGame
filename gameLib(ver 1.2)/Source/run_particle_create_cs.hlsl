#include"run_particle_cs_function.hlsli"
RWByteAddressBuffer rwBuffer:register(u0);
ByteAddressBuffer randBuffer:register(t0);
#define FLOAT_SIZE 4

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	Particle p;
	[unroll]
	for (int i = 0; i < 10; i++)
	{
		uint index = startIndex + DTid.x * 10 + i;

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
		p.scale = float3(2, 2, 2) * 0.01f;
		p.life = maxLife / (1 + moveType);
		uint bufferIndex = index * 21 * FLOAT_SIZE;
		rwBuffer.Store4(bufferIndex, asuint(p.position));
		rwBuffer.Store3(bufferIndex + FLOAT_SIZE * 4, asuint(p.scale));
		rwBuffer.Store3(bufferIndex + FLOAT_SIZE * 7, asuint(p.angle));
		rwBuffer.Store4(bufferIndex + FLOAT_SIZE * 10, asuint(p.color));
		rwBuffer.Store3(bufferIndex + FLOAT_SIZE * 14, asuint(p.velocity));
		rwBuffer.Store3(bufferIndex + FLOAT_SIZE * 17, asuint(p.accel));
		rwBuffer.Store(bufferIndex + FLOAT_SIZE * 20, asuint(p.life));

	}
}