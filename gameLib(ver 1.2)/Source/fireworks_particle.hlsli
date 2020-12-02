struct Particle
{
	float3 position;
	float3 velocity;
	float3 accel;
	float scale;
	float life;
    float lifeAmoust;
	float endTime;
	float endTimer;
	float4 startColor;
	float4 endColor;
};
struct FireworksData
{
	float maxCount;
	float maxLife;
	float speed;
	float scale;
	float parsent;
	float endTimer;
	float gravity;
	float dummy;
	float4 color;
	float4 endColor;
};
struct CreateData
{
	float3 position;
	float dummy2;
	float3 velocity;
	float dummy3;
	FireworksData firework;
};
cbuffer CbCreate:register(b0)
{
	CreateData createData[30];
};
cbuffer CbUpdate:register(b1)
{
	float elapsdTime;
	float3 dummy;
};
RWStructuredBuffer< Particle>particleBuffer:register(u0);
