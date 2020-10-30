#include"run_particle_cs_function.hlsli"
#include"rand_function.hlsli"

/********************初期化用コンピュートシェーダー*********************/
[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	uint index = DTid.x + startNumber;
	uint bufferIndex = index * MAX;
	Particle p = (Particle)0;

	//何番目のボーンの当たりに出すか
	uint boneNo = index % boneNumber;

	float angle = (rand_1_normal(float2(bufferIndex % 283, bufferIndex % 512), 1) - 1) * 3.14f;
	float z = rand_1_normal(float2(bufferIndex % 435, bufferIndex % 125), 1) - 1;
	float x = sqrt(1 - z * z) * cos(angle);
	float y = sqrt(1 - z * z) * sin(angle);
	y += 1.f;
	//初期化
	p.position = boneWorld[boneNo].xyz + float3(x, y, z)*3;
	p.velocity = float3(0, 0, -300) + float3(x, y, z) * 30* rand_1_normal(float2(bufferIndex % 275, bufferIndex % 666), 0.3f);
	p.life = 111;
	p.lifeAmount = 1 / 0.005f;
	p.color = float4(0, 1, 0, 1);
	p.scale = 0.2f;
	WriteParticle(p, bufferIndex);
}