#include"stage_obj_particle_cs_function.hlsli"

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	uint index = 100 * DTid.x + startIndex;

	uint bufferIndex = index * 19 * FLOAT_SIZE;

	Particle p;

	p = Init(DTid.x, index);

	rwBuffer.Store4(bufferIndex, asuint(p.position));
	rwBuffer.Store(bufferIndex + 4 * FLOAT_SIZE, asuint(p.life));
	rwBuffer.Store3(bufferIndex + 5 * FLOAT_SIZE, asuint(p.scale));
	rwBuffer.Store4(bufferIndex + 8 * FLOAT_SIZE, asuint(p.color));
	rwBuffer.Store3(bufferIndex + 12 * FLOAT_SIZE, asuint(p.velocity));
	rwBuffer.Store(bufferIndex + 15 * FLOAT_SIZE, asuint(p.colorType));
	rwBuffer.Store3(bufferIndex + 16 * FLOAT_SIZE, asuint(p.angle));

}