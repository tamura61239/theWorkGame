struct Particle
{
	float3 position;
	float3 accel;
	float3 velocity;
	float life;
	float4 color;
	float3 scale;
};
//定数バッファのデータ
cbuffer CbCreate:register(b0)
{
	int startIndex;
	float3 createCentralPosition;
	float randX;
	float3 createArea;
	float4 color;
	float maxLife;
	float scale;
	float2 colorRatio;
	float4 color2;
}
cbuffer CbUpdate : register(b1)
{
	float elapsdTime;
	float3 windDirection;
	float maxSpeed;
	float3 dummy2;
}
//パーティクルのデータ
RWStructuredBuffer<Particle> particleBuffer : register(u0);
//パーティクルの数
RWByteAddressBuffer particleCountBuffer : register(u1);
//パーティクルのindexデータ
RWByteAddressBuffer indexBuffer : register(u3);
RWByteAddressBuffer newIndexBuffer : register(u4);
RWStructuredBuffer<uint> deleteIndexBuffer : register(u5);
