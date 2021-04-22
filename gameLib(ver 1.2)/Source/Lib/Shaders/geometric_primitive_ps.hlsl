//float4 main() : SV_TARGET
//{
//	return float4(1.0f, 1.0f, 1.0f, 1.0f);
//}
#include "geometric_primitive.hlsli" 
float4 main(VS_OUT pin) : SV_TARGET
{
	return pin.color;
}