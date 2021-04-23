#include"run_particle_cs_function.hlsli"
#include"render_particle_cube.hlsli"
/****************************************************************************/
//　　　パーティクルの更新をする
/****************************************************************************/

[numthreads(100, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    //パーティクルの最大数を取得する
    uint aliveCount = particleCount.Load(0);
    if (aliveCount > DTid.x)
    {//パーティクルの最大数より小さいとき
        //indexバッファに登録している番号のパーティクルデータを取得する
        uint index;
        index=particleIndex.Load(DTid.x * 4);
        Particle p = particle[index];
        if (p.life > 0)
        {//寿命がまだあるなら
            float4 particleColor = (float4) 0;
            //uint型にした色データをfloat4で取得する
            particleColor.r = ((p.color >> 24) & 0x000000FF) / 255.0f;
            particleColor.g = ((p.color >> 16) & 0x000000FF) / 255.0f;
            particleColor.b = ((p.color >> 8) & 0x000000FF) / 255.0f;
            particleColor.a = ((p.color >> 0) & 0x000000FF) / 255.0f;
            //更新
            p.position += p.velocity * elapsdTime;
            p.life -= elapsdTime;
            particleColor.a = p.life;
            //色データをfloat4からuint型にする
            p.color |= ((uint) (particleColor.r * 255) & 0x000000FF) << 24;
            p.color |= ((uint) (particleColor.g * 255) & 0x000000FF) << 16;
            p.color |= ((uint) (particleColor.b * 255) & 0x000000FF) << 8;
            p.color |= ((uint) (particleColor.a * 255) & 0x000000FF) << 0;

            particle[index] = p;
            //描画用データの更新
            ParticleRender render = (ParticleRender) 0;
            render.position = float4(p.position, 1);
            render.scale = p.scale;
            render.color = particleColor;
            render.velocity = p.velocity;
            WriteRender(render, index * 17 * 4);

            //生きてる分カウントを増やす
            uint newAliveCount;
            particleCount.InterlockedAdd(4, 1, newAliveCount);
            particleNewIndex.Store(newAliveCount * 4, index);

        }
        else
        {
            //死んでる分のカウントを増やす
            uint deleteCount;
            particleCount.InterlockedAdd(4 * 2, 1, deleteCount);
            deleteIndex[deleteCount] = index;

        }

    }

}
