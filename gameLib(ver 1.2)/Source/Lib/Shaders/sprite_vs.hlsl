#include "sprite.hlsli"   
/*****************************************************/
//　　　　	2次元の座標を取得する
/*****************************************************/

VS_OUT main(float4 pos : POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD/*UNIT.04*/)
{
	VS_OUT vout;
	vout.pos = pos;
	vout.color = color;
	vout.texcoord = texcoord;
	return vout;
}