#include"respond_particle.hlsli"
#include"render_particle_cube.hlsli"
#include"particle_count_buffer.hlsli"
[numthreads(100, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	//パーティクルの最大数を取得する
	uint aliveCount = particleCountBuffer.Load(0);
	if (aliveCount > DTid.x)
	{
		float3 position = (float3)0;
		//indexバッファに登録している番号のパーティクルデータを取得する
		uint index;
		index = particleIndexBuffer.Load(DTid.x * 4);
		Particle p = particle[index];
		if (p.timer < p.respondTime + p.expansionTime)
		{
			p.timer += elapsdTime;

			if (p.timer <= p.expansionTime)
			{
				p.start += p.velocity * elapsdTime;
				p.velocity.y += -9.8f * elapsdTime;
				position = p.start;
			}
			else
			{
				float timer = p.timer - p.expansionTime;
				timer = min(timer, p.respondTime);
				
				float parsent = lerp(InCubic(timer, p.respondTime, 1, 0), OutCubic(timer, p.respondTime, 1, 0), step(p.respondTime * 0.5f, timer));
				position = lerp(p.start, p.end, parsent);
			}
			particle[index] = p;
			ParticleRender render = (ParticleRender)0;
			render.position = float4(position, 1);
			render.scale = p.scale;
			render.color = p.color;
			render.velocity = p.velocity;
			WriteRender(render, index * 17 * 4);

			//生きてる分カウントを増やす
			uint newAliveCount;
			particleCountBuffer.InterlockedAdd(4, 1, newAliveCount);
			newIndexBuffer.Store(newAliveCount * 4, index);

		}
		else
		{
			//死んでる分のカウントを増やす
			uint deleteCount;
			particleCountBuffer.InterlockedAdd(4 * 2, 1, deleteCount);
			deleteIndexBuffer[deleteCount] = index;
		}
	}
}