#include"title_texture_particle.hlsli"
#include"rand_function.hlsli"

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	//uv座標を求める
	float2 texcoord;
	texcoord.x = DTid.x % (uvSize.x / screenSplit);
	texcoord.y = DTid.x / (uvSize.x / screenSplit);

	Particle p = (Particle)0;
	//テクスチャから色を取得
	p.color = scene.Load(uint3(texcoord* screenSplit, 0));

	float4 localPosition = (float4)0;
	localPosition.x = texcoord.x/ (uvSize.x / screenSplit) * 2 - 1;
	localPosition.y = -texcoord.y/ (uvSize.y / screenSplit) * 2 + 1;
	localPosition.w = 1;

	float4 worldPosition = mul(localPosition, world);

	p.position = worldPosition.xyz;
	p.speed = rand_1_normal(float2(DTid.x % 666, DTid.x % 294), 1) +0.2f;
	float l = -length(float3(0, 0, 0) - p.position);
	l = max(l + 200, 10);
	p.speed *= l;
	particleBuffer[startIndex+DTid.x] = p;
	
}