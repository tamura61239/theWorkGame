RWTexture2D<float4> conversionTex:register(u0);
Texture2D<float4> readZTex:register(t0);
cbuffer CbData:register(b0)
{
	float inFocusMin;
	float inFocusMax;
	float2 dummy;
}
[numthreads(10, 10, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	float z = readZTex[DTid.xy].r;
	float a = 1.0f;
	float minParsent = 1 / inFocusMin;
	float maxParsent = 1 / (1 - inFocusMax);
	if (z <= inFocusMin)a = minParsent * z;
	else if (z >= inFocusMax)a = maxParsent * (1 - z);
	conversionTex[DTid.xy] = a;
}