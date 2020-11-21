struct Particle
{
	float3 position;
	float3 velocity;
	float4 color;
	float speed;
};

cbuffer CbCamera:register(b0)
{
	row_major float4x4 inverseViewProjection;
	float ndcZ;

}
cbuffer CbUpdate : register(b1)
{
	float elapsdTime;
	float scale;
	float speed;
	float2 spiralRatio;

}
Texture2D<float4>scene : register(t0);
RWStructuredBuffer< Particle>particleBuffer:register(u0);