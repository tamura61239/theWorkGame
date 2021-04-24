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
//定数バッファのデータ
cbuffer CbStart:register(b0)
{
	float3 angleMovement;
	float speed;
	float4 color;
    float sinLeng;
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
//パーティクルのデータ
RWStructuredBuffer<Particle> particleBuffer : register(u0);
