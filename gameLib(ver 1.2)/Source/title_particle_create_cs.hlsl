#include"title_psrticle_compute.hlsli"
#include"rand_function.hlsli"

/****************************************************************************/
//　　　パーティクルを生成する
/****************************************************************************/
[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	//index値の取得
	uint index = startIndex + DTid.x;
	uint bufferIndex = index * MAX;
	//データの初期化
	Particle p = (Particle)0;

	//x,y,zの値を疑似乱数を使って出す
	float x = rand_1_normal(float2(index % 591, index % 256), 1) * sphereRatio.x;
	float y = rand_1_normal(float2(index % 777, index % 444), 1) * sphereRatio.y;
	float z = rand_1_normal(float2(index % 321, index % 666), 1) * sphereRatio.z;
	//出した値を-1～1の間に収める
	x -= sphereRatio.x;
	y -= sphereRatio.y;
	z -= sphereRatio.z;
	//計算したx,y,zの値を使って初期座標を出す
	p.position = normalize(float3(x, y, z)) * rand_1_normal(float2(index % 673, index % 912),0.5f) * leng + startPosition;

	p.scale = float3(1, 1, 1) * (rand_1_normal(float2(index % 345, index % 190), 0.5f))* randScale;
	p.speed = rand_1_normal(float2(index % 666, index % 294), 1)* randSpeed + defSpeed;

	p.life = life;
	p.color = color;
	p.maxLife = p.life;
	p.defVelocity = defVelocity;
	p.defPosition = p.position;
	p.startPosition = p.position;

	p.moveAngleMovement = (rand_1_normal(float2(index % 556, index % 442), 1) - 1)* randMoveAngle;

	p.moveAngleLength = rand_1_normal(float2(index % 123, index % 581), 1) * randMoveLength + defMoveLength;
	
	//更新データの書き出し
	WriteParticle(p, bufferIndex);

}