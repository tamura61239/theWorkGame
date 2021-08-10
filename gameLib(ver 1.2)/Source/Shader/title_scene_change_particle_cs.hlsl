#include"title_psrticle_compute.hlsli"
#include"render_particle_cube.hlsli"
#include"particle_count_buffer.hlsli"

/****************************************************************************/
//　　　パーティクルが中央へ吸い込まれていく処理
/****************************************************************************/

[numthreads(100, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint aliveCount = particleCountBuffer.Load(0);
    if (aliveCount > DTid.x)
    { //パーティクルの最大数より小さいとき
        //indexバッファに登録している番号のパーティクルデータを取得する
        uint index;
        index = particleIndexBuffer.Load(DTid.x * 4);
        Particle p = particleBuffer[index];
        float l = length(float3(0, 0, 0) - p.position);
        if (abs(l) > 2.f)
        {
			//更新
            float3 vec = float3(0, 0, 0) - p.position;
            l = length(vec);
            vec = normalize(vec);
            float3 vec2 = cross(float3(0, 0, -1), vec);
			
            p.velocity = lerp((vec2 + vec) * p.speed * 10.f * elapsdTime, (vec2 + vec) * p.speed * l * elapsdTime, step(l, 10));
            p.position += p.velocity;
            particleBuffer[index] = p;
			//描画用データの初期化
            ParticleRender render = (ParticleRender) 0;
			//描画用データのセット
            render.position = float4(p.position, 1.0f);
            render.scale = p.scale;
            render.color = p.color;
            render.angle = p.angle;
            render.velocity = p.velocity;
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