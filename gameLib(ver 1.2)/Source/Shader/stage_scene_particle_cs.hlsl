#include"stage_scene_particle.hlsli"
#include"curl_noise.hlsli"
#include"render_particle_cube.hlsli"
#include"particle_count_buffer.hlsli"

/****************************************************************************/
//　　　パーティクルを更新する
/****************************************************************************/

[numthreads(100, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    //パーティクルの数を取得
    uint particleCount = particleCountBuffer.Load(0);
    if (particleCount > DTid.x)
    { //パーティクルの最大数より小さいとき
        //indexバッファに登録している番号のパーティクルデータを取得する
        uint particleIndex = particleIndexBuffer.Load(DTid.x * 4);
        Particle particle = particleBuffer[particleIndex];
        if (particle.life > 0)
        { //寿命がまだあるなら
            //更新
            particle.accel += windDirection * elapsdTime;
            particle.velocity += (normalize(snoise(particle.accel)) * 15 + particle.accel) * elapsdTime;
            float speed = length(particle.velocity);
            if (speed > maxSpeed)
            {
                float3 vec = normalize(particle.velocity);
                particle.velocity = vec * maxSpeed;
            }
            particle.position += particle.velocity * elapsdTime;
            particle.life -= elapsdTime;
            particleBuffer[particleIndex] = particle;
            
            ParticleRender render = (ParticleRender) 0;
            //描画用データの更新
            render.position = float4(particle.position, 1.0f);
            render.scale = particle.scale;
            render.color = particle.color;
            render.angle = float3(0, 0, 0);
            render.velocity = particle.velocity * elapsdTime;
            WriteRender(render, particleIndex*17*4);
			
            //生きてる分カウントを増やす
            uint newAliveCount;
            particleCountBuffer.InterlockedAdd(4, 1, newAliveCount);
            newIndexBuffer.Store(newAliveCount * 4, particleIndex);

        }
        else
        {
            //死んでる分のカウントを増やす
            uint deleteAliveCount;
            particleCountBuffer.InterlockedAdd(8, 1, deleteAliveCount);
            deleteIndexBuffer[deleteAliveCount] = particleIndex;
        }
    }
    

}