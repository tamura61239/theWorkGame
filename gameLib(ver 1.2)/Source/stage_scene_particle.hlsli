struct Particle
{
	float4 position;
	float life;
	float3 scale;
	float4 color;
	float3 velocity;
	float maxA;
	float3 angle;
};

ByteAddressBuffer randBuffer:register(t0);
RWByteAddressBuffer rwBuffer:register(u0);

cbuffer CbStart:register(b0)
{
	float startIndex;
	float3 startPosition;
	float maxLife;
	float3 createRange;
}
cbuffer Cb:register(b1)
{
	float elapsdTime;
	float3 angleMovement;
}
#define FLOAT_SIZE 4

