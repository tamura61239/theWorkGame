struct Particle
{
	float4 position;
	float3 angle;
	float3 angleMovement;
	float4 color;
	float3 velocity;
	float speed;
	float life;
	float sinAngle;
	float sinAngleMovement;
	float sinLength;
	float3 centerPosition;
};

cbuffer CbStart:register(b0)
{
	float3 angleMovement;
	float speed;
	float4 color;
	float startIndex;
	float3 eye;
	float range;
	float3 scope;
}

cbuffer CbUpdate:register(b1)
{
	float3 defVelocity;
	float elapsdTime;
	float3 endPosition;
	float dummy2;
}
RWByteAddressBuffer rwBuffer:register(u0);
#define POSITION_INDEX 0
#define ANGLE_INDEX 4*4
#define ANGLEMOVEMENT_INDEX 7*4
#define COLOR_INDEX 10*4
#define VELOCITY_INDEX 14*4
#define SPEED_INDEX 17*4
#define LIFE_INDEX 18*4
#define SINANGLE_INDEX 19*4
#define SINANGLEMOEMENT_INDEX 20*4
#define SINLUNGTH_INDEX 21*4
#define CENTERPOSITION_INDEX 22*4

#define MAX_INDEX 29*4

void WriteParticle(in Particle p, in float bufferIndex)
{
	rwBuffer.Store4(bufferIndex + POSITION_INDEX, asuint(p.position));
	rwBuffer.Store3(bufferIndex + ANGLE_INDEX, asuint(p.angle));
	rwBuffer.Store3(bufferIndex + ANGLEMOVEMENT_INDEX, asuint(p.angleMovement));
	rwBuffer.Store4(bufferIndex + COLOR_INDEX, asuint(p.color));
	rwBuffer.Store3(bufferIndex + VELOCITY_INDEX, asuint(p.velocity));
	rwBuffer.Store(bufferIndex + SPEED_INDEX, asuint(p.speed));
	rwBuffer.Store(bufferIndex + LIFE_INDEX, asuint(p.life));
	rwBuffer.Store(bufferIndex + SINANGLE_INDEX, asuint(p.sinAngle));
	rwBuffer.Store(bufferIndex + SINANGLEMOEMENT_INDEX, asuint(p.sinAngleMovement));
	rwBuffer.Store(bufferIndex + SINLUNGTH_INDEX, asuint(p.sinLength));
	rwBuffer.Store3(bufferIndex + CENTERPOSITION_INDEX, asuint(p.centerPosition));
}

void ReadParticle(inout Particle p, in float bufferIndex)
{
	p.position = asfloat(rwBuffer.Load4(bufferIndex + POSITION_INDEX));
	p.angle = asfloat(rwBuffer.Load3(bufferIndex + ANGLE_INDEX));
	p.angleMovement = asfloat(rwBuffer.Load3(bufferIndex + ANGLEMOVEMENT_INDEX));
	p.color = asfloat(rwBuffer.Load4(bufferIndex + COLOR_INDEX));
	p.velocity = asfloat(rwBuffer.Load3(bufferIndex + VELOCITY_INDEX));
	p.speed = asfloat(rwBuffer.Load(bufferIndex + SPEED_INDEX));
	p.life = asfloat(rwBuffer.Load(bufferIndex + LIFE_INDEX));
	p.sinAngle = asfloat(rwBuffer.Load(bufferIndex + SINANGLE_INDEX));
	p.sinAngleMovement = asfloat(rwBuffer.Load(bufferIndex + SINANGLEMOEMENT_INDEX));
	p.sinLength = asfloat(rwBuffer.Load(bufferIndex + SINLUNGTH_INDEX));
	p.centerPosition = asfloat(rwBuffer.Load3(bufferIndex + CENTERPOSITION_INDEX));
}
