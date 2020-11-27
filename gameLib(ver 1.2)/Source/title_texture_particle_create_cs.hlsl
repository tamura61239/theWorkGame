#include"title_texture_particle.hlsli"
#include"rand_function.hlsli"
[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	float num = rand_1_normal(float2(DTid.x % 152, DTid.x % 231), 1) * ((uvSize.x / screenSplit) * (uvSize.y / screenSplit));
	uint index = (uint)num % ((uvSize.x / screenSplit) * (uvSize.y / screenSplit));
	//uv座標を求める
	float2 texcoord;
	texcoord.x = index % (uvSize.x / screenSplit);
	texcoord.y = index / (uvSize.x / screenSplit);

	Particle p = (Particle)0;
	//テクスチャから色を取得
	p.color = scene.Load(uint3(texcoord* screenSplit, 0));

	float4 localPosition = (float4)0;
	localPosition.x = texcoord.x/ (uvSize.x / screenSplit) * 2 - 1;
	localPosition.y = -texcoord.y/ (uvSize.y / screenSplit) * 2 + 1;
	localPosition.w = 1;

	float4 worldPosition = mul(localPosition, world);

	p.position = worldPosition.xyz;
	p.speed = 40.f;
	float3 v = normalize(mul((float3)localPosition, (float3x3)world));
	p.velocity = v;
	uint bufferIndex = startIndex + DTid.x;
	particleBuffer[bufferIndex] = p;
	
}