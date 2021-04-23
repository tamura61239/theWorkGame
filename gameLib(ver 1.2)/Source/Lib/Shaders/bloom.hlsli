struct VS_OUT
{
	float4 position:SV_POSITION;
	float2 texcoord:TEXCOORD;
};
//定数バッファのデータ
cbuffer Cbuffer:register(b0)
{
	float threshold;
	float widthBlur;
	float hightBlur;
	float blurCount;
}
cbuffer CbBler:register(b1)
{
	float4 offset[16];
}