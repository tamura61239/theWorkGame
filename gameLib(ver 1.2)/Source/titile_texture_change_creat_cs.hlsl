#include"title_texture_particle.hlsli"
#include"Lib/Shaders/rand_function.hlsli"
/****************************************************************************/
//　　　パーティクルを更新
/****************************************************************************/

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	//uv座標を求める
	float2 texcoord;
	texcoord.x = DTid.x % (uvSize.x / screenSplit);
	texcoord.y = DTid.x / (uvSize.x / screenSplit);

	Particle p = (Particle)0;
	//テクスチャから色を取得
	p.color = scene.Load(uint3(texcoord * screenSplit, 0));
	//パーティクルを初期化する
	float4 localPosition = (float4)0;
	localPosition.x = texcoord.x / (uvSize.x / screenSplit) * 2 - 1;
	localPosition.y = -texcoord.y / (uvSize.y / screenSplit) * 2 + 1;
	localPosition.w = 1;

	float4 worldPosition = mul(localPosition, world);

	p.position = worldPosition.xyz;
	p.speed = rand_1_normal(float2(DTid.x % 666, DTid.x % 294), 1) + 0.2f;
	float l = -length(float3(0, 0, 0) - p.position);
	l = (l + 200) * 1.25f;
	p.speed *= l;
	//初期化したデータをセットする
	particleBuffer[startIndex + DTid.x] = p;

}