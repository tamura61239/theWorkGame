struct Particle
{
	float3 position;
	float3 accel;
	float3 velocity;
	float life;
	float4 color;
	float3 scale;
};
cbuffer CbCreate:register(b0)
{
	int startIndex;
	float3 createCentralPosition;
	float randX;
	float3 createArea;
	float4 color;
	float maxLife;
	float scale;
	float2 dummy;
}
cbuffer CbUpdate : register(b1)
{
	float elapsdTime;
	float3 windDirection;
	float maxSpeed;
	float3 dummy2;
}
#define PARTICLE_MAX 17*4

RWByteAddressBuffer rwBuffer:register(u0);

void ReadParticle(inout Particle p, in uint bufferIndex)
{
	p.position = asfloat(rwBuffer.Load3(bufferIndex + 0*4));
	p.accel = asfloat(rwBuffer.Load3(bufferIndex + 3 * 4));
	p.velocity = asfloat(rwBuffer.Load3(bufferIndex + 6 * 4));
	p.life = asfloat(rwBuffer.Load(bufferIndex + 9 * 4));
	p.color = asfloat(rwBuffer.Load4(bufferIndex + 10 * 4));
	p.scale = asfloat(rwBuffer.Load3(bufferIndex + 14 * 4));
}

void WriteParticle(in Particle p, in uint bufferIndex)
{
	rwBuffer.Store3(bufferIndex + 0 * 4, asuint(p.position));
	rwBuffer.Store3(bufferIndex + 3 * 4, asuint(p.accel));
	rwBuffer.Store3(bufferIndex + 6 * 4, asuint(p.velocity));
	rwBuffer.Store(bufferIndex + 9 * 4, asuint(p.life));
	rwBuffer.Store4(bufferIndex + 10 * 4, asuint(p.color));
	rwBuffer.Store3(bufferIndex + 14 * 4, asuint(p.scale));
}