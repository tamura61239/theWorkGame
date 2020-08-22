
RWByteAddressBuffer redObjsBuffer:register(u0);
RWByteAddressBuffer blueObjsBuffer:register(u1);
RWByteAddressBuffer rwBuffer:register(u2);

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
#define FLOAT_SIZE 4//floatŒ^‚ÌƒTƒCƒY

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

Particle Init(uint objNumber)
{
	uint objType = step(redNumber - 1, objNumber);
	float3 maxPosition, minPosition;
	//switch (objType)
	//{
	//case 0:
	//	maxPosition = asfloat(redObjsBuffer.Load3(objNumber));
	//	minPosition = asfloat(redObjsBuffer.Load3(objNumber+3*FLOAT_SIZE));
	//	break;
	//case 1:
	//	maxPosition = asfloat(blueObjsBuffer.Load3(objNumber- redNumber));
	//	minPosition = asfloat(blueObjsBuffer.Load3(objNumber- redNumber + 3 * FLOAT_SIZE));
	//	break;
	//}
	uint buffer = 0;
	if (objType == 0)
	{
		buffer = objNumber * 6 * FLOAT_SIZE;
		maxPosition = asfloat(redObjsBuffer.Load3(buffer));
		minPosition = asfloat(redObjsBuffer.Load3(buffer + 3 * FLOAT_SIZE));
	}
	if (objType == 1)
	{
		buffer = objNumber - redNumber - 1;
		buffer *= 6 * FLOAT_SIZE;
		maxPosition = asfloat(blueObjsBuffer.Load3(buffer));
		minPosition = asfloat(blueObjsBuffer.Load3(buffer + 3 * FLOAT_SIZE));
	}
	float3 range = maxPosition - minPosition;
	float3 position = minPosition + range/2;
	Particle p;
	p.colorType = objType + nowColorType;
	if (p.colorType >= 2)p.colorType -= 2;
	p.color = lerp(redColor, blueColor, p.colorType);
	p.position.xyz = position;
	p.position.w = 1.0f;
	p.scale = float3(3, 3, 3);
	p.life = 1.0f;
	p.velocity = /*normalize(position - p.position.xyz) * float3(5, 0, 5)*/ + lerp(float3(0, 10, 0), float3(0, -10, 0), p.colorType);
	p.angle = (float3)0;
	return p;
}