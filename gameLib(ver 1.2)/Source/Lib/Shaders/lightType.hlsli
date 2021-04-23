//#include"ps_function.hlsli"
//----------------------
//ポイントライト構造体
//---------------------
struct POINTLIGHT
{
	float dumy1;
	float range;
	float type;
	float dumy2;
	float4 position;
	float4 color;
};
#define POINTMAX 32
//----------------------
//スポットライト構造体
//----------------------
struct SPOTLIGHT
{
	float index;
	float range;//光の届く範囲
	float type;//有効か無効か
	float inner_com;
	float outer_com;
	float dumy0;
	float dumy1;
	float dumy2;
	float4 position;
	float4 color;
	float4 dir;
};
//定数バッファのデータ
#define SPOTMAX 32
cbuffer CbLight:register(b3)
{
	POINTLIGHT pointLight[POINTMAX];
	SPOTLIGHT spotLight[SPOTMAX];
}
cbuffer CbDefLight : register(b4)
{
	float4 lightColor;
	float4 lightDirection;
	float4 ambientColor;
	float4 eyePosition;
	float3 skyColor;
	float dummy1;
	float3 groundColor;
	float dummy2;
}
