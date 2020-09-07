#include"stage_scene_particle.hlsli"
#include"curl_noise.hlsli"

[numthreads(100, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	uint index =  DTid.x+ DTid.y+ DTid.z;
	uint bufferIndex = index * 19 * FLOAT_SIZE;

	Particle p;
	p.position = asfloat(rwBuffer.Load4(bufferIndex));
	p.life = asfloat(rwBuffer.Load(bufferIndex + 4 * FLOAT_SIZE));
	p.scale = asfloat(rwBuffer.Load3((bufferIndex + 5 * FLOAT_SIZE)));
	p.color = asfloat(rwBuffer.Load4((bufferIndex + 8 * FLOAT_SIZE)));
	p.velocity = asfloat(rwBuffer.Load3((bufferIndex + 12 * FLOAT_SIZE)));
	p.maxA = asfloat(rwBuffer.Load((bufferIndex + 15 * FLOAT_SIZE)));
	p.angle = asfloat(rwBuffer.Load3((bufferIndex + 16 * FLOAT_SIZE)));

	p.velocity += normalize(snoise(p.velocity)) * elapsdTime*15*5;
	p.position.xyz += p.velocity * elapsdTime;
	p.angle += angleMovement * elapsdTime;
	float3 cosf = float3(cos(p.angle.x), cos(p.angle.y), cos(p.angle.z));
	float3 sinf = float3(sin(p.angle.x), sin(p.angle.y), sin(p.angle.z));

	//float x = (sinf.x * cosf.y) - (cosf.x * sinf.y * sinf.y - sinf.x * cosf.z);
	//float z = (cosf.y * sinf.z) - (sinf.x * sinf.y * cosf.z - cosf.x * sinf.z);
	//p.velocity += float3(x, 0, z) * elapsdTime * 60;
	p.life -= elapsdTime;

	//p.color.a = min(maxLife - p.life, 1.5)*0.3;
	//p.color.a = min(p.life, 1.5)*0.3;
	p.color.a = lerp(min(maxLife - p.life, 0.6), min(p.life, 0.6), step(maxLife, p.life)) * p.maxA;

	rwBuffer.Store4(bufferIndex, asuint(p.position));
	rwBuffer.Store(bufferIndex + 4 * FLOAT_SIZE, asuint(p.life));
	rwBuffer.Store3(bufferIndex + 5 * FLOAT_SIZE, asuint(p.scale));
	rwBuffer.Store4(bufferIndex + 8 * FLOAT_SIZE, asuint(p.color));
	rwBuffer.Store3(bufferIndex + 12 * FLOAT_SIZE, asuint(p.velocity));
	rwBuffer.Store3(bufferIndex + 16 * FLOAT_SIZE, asuint(p.angle));

}