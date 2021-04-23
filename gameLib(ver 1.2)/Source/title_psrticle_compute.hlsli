//パーティクル情報
struct Particle
{
	float3 position;
	float3 scale;
	float3 velocity;
	float3 angle;
	float4 color;
	float speed;
	float life;
	float moveAngle;
	float moveAngleMovement;
	float moveAngleLength;
	float maxLife;
	float3 defVelocity;
	float3 startPosition;
	float3 defPosition;
};
//定数バッファのデータ
cbuffer CbStart:register(b0)
{
	float startIndex;
	float3 startPosition;
	float leng;
	float3 sphereRatio;
	float4 color;
	float life;
	float moveLen;
	float randSpeed;
	float defSpeed;
}
cbuffer CbStart2 :register(b1)
{
	float randMoveLength;
	float defMoveLength;
	float randMoveAngle;
	float randScale;
	float3 defVelocity;
	float dummy;
}
cbuffer CbUpdate:register(b2)
{
	float elapsdTime;
	float3 angleMovement;
}
//indexサイズ
#define POSITION_INDEX 0
#define SCALE_INDEX 3*4
#define VELOCITY_INDEX 6*4
#define ANGLE_INDEX 9*4
#define COLOR_INDEX 12*4
#define SPEED_INDEX 16*4
#define LIFE_INDEX 17*4
#define MOVEANGLE_INDEX 18*4
#define MOEANGLEMOVEMENT_INDEX 19*4
#define MOEANGLELENGTH_INDEX 20*4
#define MAXLIFE_INDEX 21*4
#define DEFVELOCITY_INDEX 22*4
#define STARTPOSITION_INDEX 25*4
#define DEFPOSITION_INDEX 28*4
#define MAX 31*4
//パーティクル情報バッファ
RWByteAddressBuffer rwBuffer:register(u0);
//パーティクル情報の読み込み
void ReadParticle(inout Particle p, in uint bufferIndex)
{
	p.position = asfloat(rwBuffer.Load3(bufferIndex + POSITION_INDEX));
	p.scale = asfloat(rwBuffer.Load3(bufferIndex + SCALE_INDEX));
	p.velocity = asfloat(rwBuffer.Load3(bufferIndex + VELOCITY_INDEX));
	p.angle = asfloat(rwBuffer.Load3(bufferIndex + ANGLE_INDEX));
	p.color = asfloat(rwBuffer.Load4(bufferIndex + COLOR_INDEX));
	p.speed = asfloat(rwBuffer.Load(bufferIndex + SPEED_INDEX));
	p.life = asfloat(rwBuffer.Load(bufferIndex + LIFE_INDEX));
	p.moveAngle = asfloat(rwBuffer.Load(bufferIndex + MOVEANGLE_INDEX));
	p.moveAngleMovement = asfloat(rwBuffer.Load(bufferIndex + MOEANGLEMOVEMENT_INDEX));
	p.moveAngleLength = asfloat(rwBuffer.Load(bufferIndex + MOEANGLELENGTH_INDEX));
	p.maxLife = asfloat(rwBuffer.Load(bufferIndex + MAXLIFE_INDEX));
	p.defVelocity = asfloat(rwBuffer.Load3(bufferIndex + DEFVELOCITY_INDEX));
	p.startPosition = asfloat(rwBuffer.Load3(bufferIndex + STARTPOSITION_INDEX));
	p.defPosition = asfloat(rwBuffer.Load3(bufferIndex + DEFPOSITION_INDEX));
}
//パーティクル情報の書き出し
void WriteParticle(in Particle p, in uint bufferIndex)
{
	rwBuffer.Store3(bufferIndex + POSITION_INDEX, asuint(p.position));
	rwBuffer.Store3(bufferIndex + SCALE_INDEX, asuint(p.scale));
	rwBuffer.Store3(bufferIndex + VELOCITY_INDEX, asuint(p.velocity));
	rwBuffer.Store3(bufferIndex + ANGLE_INDEX, asuint(p.angle));
	rwBuffer.Store4(bufferIndex + COLOR_INDEX, asuint(p.color));
	rwBuffer.Store(bufferIndex + SPEED_INDEX, asuint(p.speed));
	rwBuffer.Store(bufferIndex + LIFE_INDEX, asuint(p.life));
	rwBuffer.Store(bufferIndex + MOVEANGLE_INDEX, asuint(p.moveAngle));
	rwBuffer.Store(bufferIndex + MOEANGLEMOVEMENT_INDEX, asuint(p.moveAngleMovement));
	rwBuffer.Store(bufferIndex + MOEANGLELENGTH_INDEX, asuint(p.moveAngleLength));
	rwBuffer.Store(bufferIndex + MAXLIFE_INDEX, asuint(p.maxLife));
	rwBuffer.Store3(bufferIndex + DEFVELOCITY_INDEX, asuint(p.defVelocity));
	rwBuffer.Store3(bufferIndex + STARTPOSITION_INDEX, asuint(p.startPosition));
	rwBuffer.Store3(bufferIndex + DEFPOSITION_INDEX, asuint(p.defPosition));
}