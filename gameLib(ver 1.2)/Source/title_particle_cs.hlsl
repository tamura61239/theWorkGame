#include"title_psrticle_compute.hlsli"
#include"render_particle_cube.hlsli"
#include"particle_count_buffer.hlsli"

/****************************************************************************/
//　　　パーティクルを更新
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
		if(p.life>0)
        {
			//更新
            p.moveAngle += p.moveAngleMovement * elapsdTime;
			
            p.defPosition += p.defVelocity * p.speed * elapsdTime;
            p.life -= elapsdTime;
			
            float3 right = cross(normalize(p.defVelocity), float3(1, 1, 0));
			
            p.moveAngle += p.moveAngleMovement * elapsdTime;
			
            p.position = p.defPosition + right * sin(p.moveAngle) * p.moveAngleLength;
			
            p.life -= elapsdTime;
			
            particleBuffer[index] = p;
            ParticleRender render = (ParticleRender) 0;
			//描画用データの更新
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