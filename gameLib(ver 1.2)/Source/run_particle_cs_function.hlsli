cbuffer Cb:register(b0)
{
	float3 angleMovement;
	float elapsdTime;
	float startIndex;
	float indexSize;
	float createFlag;
	float dummy2;
}
cbuffer CbStart:register(b1)
{
	float4 playerPosition;
	float4 color;
	float3 playerVelocity;
	float maxLife;
	float rand1;
	float rand2;
	float totalRand;
	float moveType;
}
struct Particle
{
	float4 position;
	float3 scale;
	float3 angle;
	float4 color;
	float3 velocity;
	float3 accel;
	float life;
};