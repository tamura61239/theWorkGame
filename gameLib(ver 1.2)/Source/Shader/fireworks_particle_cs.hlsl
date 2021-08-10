#include"fireworks_particle.hlsli"
#include"render_particle_cube.hlsli"
#include"particle_count_buffer.hlsli"
/****************************************************************************/
//　　　花火のパーティクルの更新
/****************************************************************************/

[numthreads(100, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	    //パーティクルの最大数を取得する
    uint aliveCount = particleCountBuffer.Load(0);
    if (aliveCount > DTid.x)
    { //パーティクルの最大数より小さいとき
        //indexバッファに登録している番号のパーティクルデータを取得する
        uint index;
        index = particleIndexBuffer.Load(DTid.x * 4);
        Particle p = particleBuffer[index];
        float4 color = p.startColor;
        color.a = lerp(1, 0, step(p.life, 0) * (p.endTime / p.endTimer));

        if (color.a > 0)
        {
            float3 velocity = p.velocity;
            p.velocity += p.accel * pow(1 - p.life * (p.endTimer - p.endTime) / p.endTimer, 2) * elapsdTime;
            p.velocity.xz = lerp(velocity.xz, p.velocity.xz, step(abs(p.velocity.xz - velocity.xz), abs(p.velocity.xz - 0)));
            p.position += p.velocity * elapsdTime;
            p.life -= p.lifeAmoust * elapsdTime;
            p.endTime += step(p.life, 0) * elapsdTime;
            
            particleBuffer[index] = p;
            //描画用データの初期化
            ParticleRender render = (ParticleRender) 0;
            //描画用データのセット
            render.position = float4(p.position, color.a);
            render.scale = p.scale;
            render.color = color;
            render.velocity = p.velocity * elapsdTime;
            uint bufferIndex = index * 17 * 4;
            //描画用データの書き出し
            WriteRender(render, bufferIndex);
            
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