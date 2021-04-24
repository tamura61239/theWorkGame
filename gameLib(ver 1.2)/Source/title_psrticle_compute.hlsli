//パーティクル情報
struct Particle
{
	float3 position;
	float3 scale;
	float3 velocity;
	float3 angle;
	float4 color;
	float speed;
	float life;
	float moveAngle;
	float moveAngleMovement;
	float moveAngleLength;
	float maxLife;
	float3 defVelocity;
	float3 startPosition;
	float3 defPosition;
};
//定数バッファのデータ
cbuffer CbStart:register(b0)
{
	float startIndex;
	float3 startPosition;
	float leng;
	float3 sphereRatio;
	float4 color;
	float life;
	float moveLen;
	float randSpeed;
	float defSpeed;
}
cbuffer CbStart2 :register(b1)
{
	float randMoveLength;
	float defMoveLength;
	float randMoveAngle;
	float randScale;
	float3 defVelocity;
	float dummy;
}
cbuffer CbUpdate:register(b2)
{
	float elapsdTime;
	float3 angleMovement;
}
//パーティクル情報バッファ
RWStructuredBuffer<Particle> particleBuffer : register(u0);
