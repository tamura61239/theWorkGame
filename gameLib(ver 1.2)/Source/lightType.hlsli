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
#define SPOTMAX 32
cbuffer CbLight:register(b3)
{
	POINTLIGHT pointLight[POINTMAX];
	SPOTLIGHT spotLight[SPOTMAX];
}

struct Light
{
	float3 PD;
	float3 PS;
};
/****************************************/
//ポイントライト計算関数
//pointLight:ポイントライト
//worldPosition:ワールド座標
//normal:法線
//Kd:反射率(0～1.0)
//Ks:入射光(色・強さ)
//E:視点方向ベクトル(正規化済み)
/****************************************/
//Light CalculationPL(POINTLIGHT pointLight, float3 normal
//	, float3 Kd, float3 Ks, float3 E,float3 PL, float d, float r)
//{
//	float influence;  //ポイントライトの影響
//	float3 PC;    //ポイントライト色
//	influence = saturate(1.0f - d / r);
//	PL = normalize(PL);
//	PC = pointLight.color.rgb;
//	Light light;
//	//拡散光の加算
//	//light.PD = Diffuse(N, PL, PC, Kd) * influence * influence;
//	////鏡面光の加算
//	//light.PS = BlinnPhongSpcular(N, PL, PC, E, Ks, 20) * influence * influence;
//	return light;
//}
//Light IsPLt(POINTLIGHT pointLight, float3 worldPosition, float3 normal
//	, float3 Kd, float3 Ks, float3 E)
//{
//	float3 PL;   //ポイントライトベクトル
//	PL = worldPosition - pointLight.position.xyz;
//	float d = length(PL);
//	float r = pointLight.range;
//	return lerp(CalculationPL(pointLight, normal, Kd, Ks, E,PL, d, r), (Light)0, step(r, d));
//}
//
//
//Light ChackPL(POINTLIGHT pointLight,float3 worldPosition,float3 normal
//	,float3 Kd,float3 Ks,float3 E)
//{
//	return lerp((Light)0, IsPLt(pointLight, worldPosition, normal, Kd, Ks, E), pointLight.type);
//}

