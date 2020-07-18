//#include"lightType.hlsli"
//#include "static_mesh.hlsli"
//
//#include"ps_function.hlsli"
//struct Light
//{
//	float3 PD;
//	float3 PS;
//};
///****************************************/
////ポイントライト計算関数
////pointLight:ポイントライト
////worldPosition:ワールド座標
////normal:法線
////Kd:反射率(0～1.0)
////Ks:入射光(色・強さ)
////E:視点方向ベクトル(正規化済み)
///****************************************/
//Light ChackPL(POINT_LIGHT pointLight,float3 worldPosition,float3 normal
//	,float3 Kd,float3 Ks,float3 E)
//{
//	return lerp((Light)0, IsPLt(pointLight, worldPosition, normal, Kd, Ks, E), pointLight.type);
//}
//
//Light IsPLt(POINT_LIGHT pointLight, float3 worldPosition, float3 normal
//	, float3 Kd, float3 Ks, float3 E)
//{
//	float3 PL;   //ポイントライトベクトル
//	float3 PC;    //ポイントライト色
//	PL = worldPosition - pointLight.position.xyz;
//	float d = length(PL);
//	float r = pointLight.range;
//	return lerp(CalculationPL(pointLight, normal, Kd, Ks, E, d, r), (Light)0, step(r, d));
//}
//
//Light CalculationPL(POINT_LIGHT pointLight, float3 normal
//	, float3 Kd, float3 Ks, float3 E,float d,float r)
//{
//	float influence;  //ポイントライトの影響
//	influence = saturate(1.0f - d / r);
//	PL = normalize(PL);
//	PC = PointLight[i].color.rgb;
//	Light light;
//	//拡散光の加算
//	light.PD = Diffuse(N, PL, PC, Kd) * influence * influence;
//	//鏡面光の加算
//	light.PS = BlinnPhongSpcular(N, PL, PC, E, Ks, 20) * influence * influence;
//	return light;
//}