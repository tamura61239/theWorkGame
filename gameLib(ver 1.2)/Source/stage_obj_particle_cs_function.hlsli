ByteAddressBuffer randBuffer:register(t0);
ByteAddressBuffer StageBuffer:register(t1);

RWByteAddressBuffer rwBuffer:register(u0);
#include"curl_noise.hlsli"

cbuffer CbBuffer:register(b0)
{
	float maxLife;
	float elapsdTime;
	float nowColorType;
	float maxSize;
	float3 angleMovement;
	float dummy2;
}

cbuffer CbStartData:register(b1)
{
	float startIndex;
	float indexSize;
	float redNumber;
	float changeColorFlag;
	float4 redColor;
	float4 blueColor;
}
#define FLOAT_SIZE 4//float型のサイズ

struct Particle
{
	float4 position;
	float life;
	float3 scale;
	float4 color;
	float3 velocity;
	float colorType;
	float3 angle;
};
//パーティクルの生成
Particle Init(uint objNumber,uint index)
{
	uint objType = step(redNumber - 1, objNumber);
	float3 maxPosition, minPosition;
	uint buffer = 0;
	buffer = objNumber * 6 * FLOAT_SIZE;
	maxPosition = asfloat(StageBuffer.Load3(buffer));
	minPosition = asfloat(StageBuffer.Load3(buffer + 3 * FLOAT_SIZE));

	buffer = index%400/* * 3 * FLOAT_SIZE*/;
	buffer *= 3 * FLOAT_SIZE;
	float3 rand = asfloat(randBuffer.Load3(buffer));

	float3 range = maxPosition - minPosition;
	float3 position = minPosition + range / 2;
	Particle p;
	p.colorType = objType + nowColorType;
	if (p.colorType >= 2)p.colorType -= 2;
	p.color = lerp(redColor, blueColor, p.colorType);
	p.position.xyz = minPosition + range*rand;
	p.position.w = 1.0f;
	p.scale = float3(3, 3, 3)*0.1f;
	p.life = 1.0f;
	p.velocity = normalize(snoise(normalize(position - p.position.xyz))) * float3(5, 0, 5) +lerp(float3(0, 10, 0), float3(0, -10, 0), p.colorType);
	p.angle = (float3)0;
	p.angle.y = radians((index + objNumber) % 360);
	p.angle.x= radians((index * objNumber) % 360);
	return p;
}