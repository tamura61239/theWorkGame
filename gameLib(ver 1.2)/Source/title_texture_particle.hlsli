struct Particle
{
	float3 position;
	float3 velocity;
	float speed;
	float4 color;
	float life;

};
cbuffer CbCreate: register(b0)
{
	row_major float4x4 inverseViewProjection;
	float2 textureSize;
	float z;
	float startIndex;
	float2 leftTop;
	float2 viewport;
}
cbuffer Cb:register(b1)
{
	float elapsdTime;
	float3 endPosition;
}
#define MAX 12*4
//パーティクル情報バッファ
RWByteAddressBuffer rwBuffer:register(u0);
//パーティクル情報の読み込み
void ReadParticle(inout Particle p, in uint bufferIndex)
{
	p.position = asfloat(rwBuffer.Load3(bufferIndex));
	p.velocity = asfloat(rwBuffer.Load3(bufferIndex+3*4));
	p.speed = asfloat(rwBuffer.Load(bufferIndex+6*4));
	p.color = asfloat(rwBuffer.Load4(bufferIndex+7*4));
	p.life = asfloat(rwBuffer.Load(bufferIndex+11*4));
}
//パーティクルの書き込み
void WriteParticle(in Particle p, in uint bufferIndex)
{
	rwBuffer.Store3(bufferIndex, asuint(p.position));
	rwBuffer.Store3(bufferIndex + 3 * 4, asuint(p.velocity));
	rwBuffer.Store(bufferIndex + 6 * 4, asuint(p.speed));
	rwBuffer.Store4(bufferIndex + 7 * 4, asuint(p.color));
	rwBuffer.Store(bufferIndex + 11 * 4, asuint(p.life));
}