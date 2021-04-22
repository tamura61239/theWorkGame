//float4 main( float4 pos : POSITION ) : SV_POSITION
//{
//	return pos;
//}
#include "geometric_primitive.hlsli"
VS_OUT main(float4 pos : POSITION, float3 normal : NORMAL)
{
	VS_OUT vout;
#if TYPE

	vout.pos = mul(pos, worldViewProjection);
	float3 N = normalize(mul(normal, (float3x3)world));
	float3 L = normalize(-lightDirection.xyz);
	float d = dot(L, N);
	vout.color.xyz = materialColor.xyz * max(0, d);
	vout.color.w = materialColor.w;
#else
	vout.pos = mul(pos, world);
	vout.pos = mul(vout.pos, view);
	vout.pos = mul(vout.pos, projection);
	float3 N = normalize(mul(normal, (float3x3)world));
	float3 L = normalize(-lightDirection.xyz);
	float d = dot(L, N);
	vout.color.xyz = materialColor.xyz * max(0, d);
	vout.color.w = materialColor.w;

#endif
	return vout;
}
