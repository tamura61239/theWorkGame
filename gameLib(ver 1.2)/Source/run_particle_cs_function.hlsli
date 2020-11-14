struct Particle
{
	float3 position;
	float3 velocity;
	float life;
	float4 color;
	float scale;
	float lifeAmount;
};

cbuffer CbBone :register(b0)
{
	float4	boneWorld[32];
	float boneNumber;
	float3 dummy1;
}

cbuffer CbCreate : register(b1)
{
	float3 velocity;
	float maxLife;
	float4 color;
	float scale;
	float startNumber;
	float speed;
	float dummy3;
}
cbuffer CbUpdate : register(b2)
{
	float elapsdTime;
	float scaleAmount;
	float2 dummy2;
}
#define POSITION_INDEX 0
#define VELOCITY_INDEX 3*4
#define LIFE_INDEX 6*4
#define COLOR_INDEX 7*4
#define SCALE_INDEX 11*4
#define LIFEAMOUNT_INDEX 12*4
#define MAX 13*4

RWByteAddressBuffer rwBuffer:register(u0);
ByteAddressBuffer meshBuffer:register(t0);

void WriteParticle(in Particle p, in uint bufferIndex)
{
	rwBuffer.Store3(bufferIndex + POSITION_INDEX, asuint(p.position));
	rwBuffer.Store3(bufferIndex + VELOCITY_INDEX, asuint(p.velocity));
	rwBuffer.Store(bufferIndex + LIFE_INDEX, asuint(p.life));
	rwBuffer.Store4(bufferIndex + COLOR_INDEX, asuint(p.color));
	rwBuffer.Store(bufferIndex + SCALE_INDEX, asuint(p.scale));
	rwBuffer.Store(bufferIndex + LIFEAMOUNT_INDEX, asuint(p.lifeAmount));
}
void ReadParticle(inout Particle p, in uint bufferIndex)
{
	p.position = asfloat(rwBuffer.Load3(bufferIndex + POSITION_INDEX));
	p.velocity = asfloat(rwBuffer.Load3(bufferIndex + VELOCITY_INDEX));
	p.life = asfloat(rwBuffer.Load(bufferIndex + LIFE_INDEX));
	p.color = asfloat(rwBuffer.Load4(bufferIndex + COLOR_INDEX));
	p.scale = asfloat(rwBuffer.Load(bufferIndex + SCALE_INDEX));
	p.lifeAmount = asfloat(rwBuffer.Load(bufferIndex + LIFEAMOUNT_INDEX));
}
