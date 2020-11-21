#include"title_texture_particle.hlsli"
#include"rand_function.hlsli"

static uint width = 1920 / 5;
static uint height = 1080 / 5;
[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	uint x = DTid.x % width;
	uint y = DTid.x / width;

	float2 uv = float2(x, y);
	float4 ndcPosition;
	ndcPosition.x = uv.x * 2/ width - 1;
	ndcPosition.y = 1 - 2 * uv.y / height;
	ndcPosition.z = (ndcZ-0)/(1-0);
	ndcPosition.w = 1;
	uv.x /= width;
	uv.y /= height;

	float4 worldPosition = mul(ndcPosition, inverseViewProjection);
	worldPosition /= worldPosition.w;
	//worldPosition.xy *= 1200;
	//worldPosition.z = ndcZ;
	Particle p = (Particle)0;
	p.color = /*float4(1, 0, 0, 1);*/scene.Load(uint3(x*5,y*5,0))*5;
	p.position = worldPosition.xyz;
	p.speed = rand_1_normal(float2(x, y), 1)*0.02f+ 0.005f;
	if (length(scene.Load(uint3(x * 5, y * 5, 0)).rgb) <= 0)
	{
		p.position *= 0;
		p.color = 0;
	}
	particleBuffer[DTid.x] = p;
}