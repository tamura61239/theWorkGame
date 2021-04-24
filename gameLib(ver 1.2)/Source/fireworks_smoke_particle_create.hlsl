#include"fireworks_particle.hlsli"
#include"Lib/Shaders/rand_function.hlsli"
#include"particle_count_buffer.hlsli"

/****************************************************************************/
//　　　花火を打ち上げてる途中の煙のパーティクルを生成する
/****************************************************************************/

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint index = DTid.x + DTid.y + DTid.z;
    uint count = 0;
    uint emitorNumber = index / 100;
    uint totalIndex = 0;
    [unroll]
    for (int i = 0; i < 30; i++)
    {
        //このフレームでパーティクルを生成するエミッターから1エミッター当たりの生成数を足す
        totalIndex += createData[i].firework.maxCount;
        //今のindexをtotalIndexで割る
        int anser = (int) (index / totalIndex);
        //割った値が1以上ならカウントを増やす
        emitorNumber +=  saturate(anser);
        count = lerp(count, totalIndex-index, saturate(anser+1));
    }
    //死んでるパーティクルの数を1つ減らす
    uint deadCount;
    particleCountBuffer.InterlockedAdd(4 * 2, -1, deadCount);
    uint newParticleIndex = deleteIndexBuffer[deadCount - 1];

    Particle p = (Particle) 0;
    p.position = createData[emitorNumber].position - createData[emitorNumber].velocity * (count / createData[emitorNumber].firework.maxCount);
    p.startColor = float4(1,1,1,1);
    p.endTimer = createData[emitorNumber].firework.endTimer;
    p.scale = createData[emitorNumber].firework.scale;

    //生成したパーティクルをバッファにセットする
    particleBuffer[newParticleIndex] = p;
    //カウントを増やす
    uint aliveCount;
    particleCountBuffer.InterlockedAdd(0, 1, aliveCount);
    particleIndexBuffer.Store(aliveCount * 4, newParticleIndex);

}