#include"static_mesh.hlsli"
float4 main(float4 position : POSITION, float3 normal : NORMAL, float2 texcoord : TEXCOORD) : SV_POSITION
{
	//float4 pos = mul(position,world);
	float4 worldPosition = mul(position,world);
	float4 viewPosition = mul(worldPosition, view);
	return mul(viewPosition, projection);
}