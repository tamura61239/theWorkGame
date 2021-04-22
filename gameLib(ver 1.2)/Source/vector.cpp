#include "vector.h"
/*****************************************************/
//　　　　　　　　　　float2の関数
/*****************************************************/
/******************√計算をする*************************/

float ToDistVec2(const VECTOR2F v)
{
	return sqrtf((v.x * v.x) + (v.y * v.y));
};
/****************************正規化**************************/

VECTOR2F NormalizeVec2(const VECTOR2F v)
{
	float dist = ToDistVec2(v);


	return VECTOR2F(v.x / dist,
		v.y / dist);
};;
/***************************(floatとの)外積を計算する******************************/

VECTOR2F ConvertVec2(const float conv, const VECTOR2F v)
{
	const VECTOR2F normalize_vec2 = NormalizeVec2(v);
	return normalize_vec2 * conv;
};
/***************************(ベクトルとの)外積を計算する******************************/

float CrossVec2(const VECTOR2F v1, const VECTOR2F v2)
{	//ベクトルの外積
	return v1.x * v2.y - v1.y * v2.x;
};
/***********************内積を計算する************************/

float DotVec2(const VECTOR2F v1, const VECTOR2F v2)
{	//ベクトルの内積
	return v1.x * v2.x + v1.y * v2.y;
};
/*****************************************************/
//　　　　　　　　　　float3の関数
/*****************************************************/

/******************√計算をする*************************/
float ToDistVec3(const VECTOR3F v)
{
	return sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}
/****************************正規化**************************/
VECTOR3F NormalizeVec3(const VECTOR3F v)
{
	float dist = ToDistVec3(v);


	return VECTOR3F(v.x / dist,
		v.y / dist,
		v.z / dist);
}
/***************************(floatとの)外積を計算する******************************/

VECTOR3F ConvertVec3(const float conv, VECTOR3F v)
{
	const VECTOR3F normalize_vec3 = NormalizeVec3(v);
	return normalize_vec3 * conv;
}
/***************************(ベクトルとの)外積を計算する******************************/
VECTOR3F CrossVec3(const VECTOR3F v1, const VECTOR3F v2)
{
	return VECTOR3F(v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x);
}
/***********************内積を計算する************************/
float DotVec3(const VECTOR3F v1, const VECTOR3F v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}