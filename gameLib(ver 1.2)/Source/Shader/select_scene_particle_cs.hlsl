#include"select_scene_particle.hlsli"
#include"render_particle_cube.hlsli"
#include"curl_noise.hlsli"
#include"matrix_calculation.hlsli"
#include"particle_count_buffer.hlsli"

/****************************************************************************/
//　　　パーティクルの更新をする
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
            particle.angle += particle.angleMovement * elapsdTime;
			
            particle.sinLength += particle.sinLength * elapsdTime * 0.25f;
            particle.sinAngle += particle.sinAngleMovement * elapsdTime;
		
            float3 vec = normalize(endPosition - particle.centerPosition);
            float3 vec2 = normalize(defVelocity);
            float3 vec3 = normalize(vec + vec2);
		
            float3 right = float3(-1, 0, 0);
		
            float3 up = cross(vec3, right);
		
            right = cross(vec3, up);
		
            particle.velocity = normalize(vec3 * particle.speed + right * sin(particle.sinAngle) * particle.sinLength) * particle.speed * 1.5f;
		
            particle.centerPosition += vec3 * particle.speed * 1.5f * elapsdTime;
		
            particle.position.xyz += particle.velocity * elapsdTime;
		
            particle.life -= elapsdTime;
		
            particleBuffer[particleIndex] = particle;
		
            ParticleRender render = (ParticleRender) 0;
		
            //描画用データの更新
            render.position = particle.position;
            render.angle = particle.angle;
            render.color = particle.color;
            render.velocity = particle.velocity;
            render.scale = float3(1, 1, 1) * 0.5f;
            WriteRender(render, particleIndex * 17 * 4);
            //生きてる分カウントを増やす
            uint newParticleCount;
            particleCountBuffer.InterlockedAdd(4, 1, newParticleCount);
            newIndexBuffer.Store(newParticleCount * 4, particleIndex);

        }
        else
        {
            //死んでる分のカウントを増やす
            uint deleteParticleCount;
            particleCountBuffer.InterlockedAdd(8, 1, deleteParticleCount);
            deleteIndexBuffer[deleteParticleCount] = particleIndex;
        }

    }
}