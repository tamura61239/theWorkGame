#include"fireworks_particle.hlsli"
#include"../Lib/Shaders/rand_function.hlsli"
#include"particle_count_buffer.hlsli"
/****************************************************************************/
//　　　花火のパーティクルを生成する
/****************************************************************************/

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	uint index = DTid.x + DTid.y + DTid.z;
	uint count = 0;
    uint totalIndex = 0;
	[unroll]
    for (int i = 0; i < 30;i++)
    {
        //このフレームでパーティクルを生成するエミッターから1エミッター当たりの生成数を足す
        totalIndex += createData[i].firework.maxCount;
        //今のindexをtotalIndexで割る
        int anser = (int)(index / totalIndex);
        //割った値が1以上ならカウントを増やす
        count += saturate(anser);
    }
    //死んでるパーティクルの数を1つ減らす
    uint deadCount;
    particleCountBuffer.InterlockedAdd(4 * 2, -1, deadCount);
    uint newParticleIndex = deleteIndexBuffer[deadCount - 1];

     Particle p = (Particle) 0;
    //球体面上の点を取得
	float angle = (rand_1_normal(float2(index % 28, index % 51), 1) - 1) * 3.14f;
	float z = rand_1_normal(float2(index % 43, index % 12), 1) - 1;
	float x = sqrt(1 - z * z) * cos(angle);
	float y = sqrt(1 - z * z) * sin(angle);
    //値をセットする
	p.position = createData[count].position;
    p.velocity = createData[count].firework.speed * float3(x, y, z) * (rand_1_normal(float2(index % 356, index % 263), 0.5f) + 0.4f);
    float velocityLength = length(p.velocity);
    p.startColor = lerp(createData[count].firework.color, createData[count].firework.endColor, step(velocityLength, createData[count].firework.speed * createData[count].firework.parsent));
	p.endColor = createData[count].firework.endColor;
	p.life = 1;
    p.lifeAmoust = 1 / createData[count].firework.maxLife;
	p.scale = createData[count].firework.scale;
	
    p.accel = createData[count].firework.speed * float3(-x * createData[count].firework.parsent, createData[count].firework.gravity, -z * createData[count].firework.parsent);
	p.endTimer = createData[count].firework.endTimer;
    //生成したパーティクルをバッファにセットする
    particleBuffer[newParticleIndex] = p;
    //カウントを増やす
    uint aliveCount;
    particleCountBuffer.InterlockedAdd(0, 1, aliveCount);
    particleIndexBuffer.Store(aliveCount * 4, newParticleIndex);
}