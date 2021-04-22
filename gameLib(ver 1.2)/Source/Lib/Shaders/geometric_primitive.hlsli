struct VS_OUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
};
#define TYPE 0
#if TYPE
cbuffer CbScene:register(b0)
{
	float4 lightDirection;
}
cbuffer CbObj:register(b1)
{
	row_major float4x4 worldViewProjection;
	row_major float4x4 world;
	float4 materialColor;
}
#else
cbuffer CbScene:register(b0)
{
	float4 lightDirection;
	row_major float4x4 view;
	row_major float4x4 projection;

}
cbuffer CbObj : register(b1)
{
	row_major float4x4 world;
	float4 materialColor;
}

#endif