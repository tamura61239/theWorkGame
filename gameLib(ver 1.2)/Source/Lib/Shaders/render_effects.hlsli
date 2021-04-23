struct VS_OUT
{
	float4 position : SV_POSITION;
	float2 texcoord: TEXCOORD;
};
struct SaveData
{
    float3 shadowColor;
    float shadowBisa;
    float slopeScaledBias;
    float depthBiasClamp;
    float2 dummy;
};
//定数バッファのデータ
cbuffer CONSTANT_BUFFER:register(b0)
{
	row_major float4x4 inverseViewProjection;
	row_major float4x4 lightViewProjection;
    SaveData data;

}
