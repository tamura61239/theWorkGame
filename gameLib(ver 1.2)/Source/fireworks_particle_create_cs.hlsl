#include"fireworks_particle.hlsli"
#include"rand_function.hlsli"

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	uint index = DTid.x + DTid.y + DTid.z;
	uint count = 0;
	uint anser = index;
	//[unroll]
	//for (int i = 0; i < 30; i++)
	//{
	//	anser -= (uint)createData[i].firework.maxCount;
	//	count += step(1, anser);
	//}
	Particle p = (Particle)0;
	float angle = (rand_1_normal(float2(index % 28, index % 51), 1) - 1) * 3.14f;
	float z = rand_1_normal(float2(index % 43, index % 12), 1) - 1;
	float x = sqrt(1 - z * z) * cos(angle);
	float y = sqrt(1 - z * z) * sin(angle);
	p.position = createData[count].position;
    p.velocity = createData[count].firework.speed * float3(x, y, z) * (rand_1_normal(float2(index % 356, index % 263), 0.5f) + 0.2f);
    float velocityLength = length(p.velocity);
    p.startColor = lerp(createData[count].firework.color, createData[count].firework.endColor, step(velocityLength, createData[count].firework.speed * createData[count].firework.parsent));
	p.endColor = createData[count].firework.endColor;
	p.life = 1;
    p.lifeAmoust = 1 / createData[count].firework.maxLife;
	p.scale = createData[count].firework.scale;
	
    p.accel = createData[count].firework.speed * float3(-x * createData[count].firework.parsent, createData[count].firework.gravity, -z * createData[count].firework.parsent);
	p.endTimer = createData[count].firework.endTimer;
	particleBuffer[index] = p;
}