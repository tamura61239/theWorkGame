#include "geometric_primitive.hlsli" 
/*****************************************************/
//　　　　ピクセル単位で色を付ける
/*****************************************************/

float4 main(VS_OUT pin) : SV_TARGET
{
	return pin.color;
}