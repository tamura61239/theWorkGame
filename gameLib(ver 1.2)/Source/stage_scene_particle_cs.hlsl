#include"stage_scene_particle.hlsli"
#include"curl_noise.hlsli"
#include"render_particle_cube.hlsli"

[numthreads(100, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	//uint index = DTid.x;
	//uint bufferIndex = index * PARTICLE_MAX;

	//Particle p = (Particle)0;

	//ReadParticle(p, bufferIndex);

	//p.accel += windDirection * elapsdTime;
	//p.velocity += (normalize(snoise(normalize(p.accel)))*15/* + p.accel*/ + p.accel) * elapsdTime;
	//float speed = length(p.velocity);
	//if (speed > maxSpeed)
	//{
	//	float3 vec = normalize(p.velocity);
	//	p.velocity = vec * maxSpeed;
	//}
	//p.position += p.velocity* elapsdTime;

	//p.life -= elapsdTime;

	//if (p.life <= 0)
	//{
	//	p.position = float3(0, 0, 0);
	//	p.velocity = float3(0, 0, 0);
	//	p.color = float4(0, 0, 0, 0);
	//	p.scale = float3(0, 0, 0);
	//}
	////�X�V�f�[�^�̏����o��
	//WriteParticle(p, bufferIndex);

	////�`��p�f�[�^�̏�����
	//ParticleRender render = (ParticleRender)0;
	////�`��p�f�[�^�̃Z�b�g
	//render.position = float4(p.position, 1.0f);
	//render.scale = p.scale;
	//render.color = p.color;
	//render.angle = float3(0,0,0);
 //   render.velocity = p.velocity * elapsdTime;
	//bufferIndex = index * 17 * 4;
	////�`��p�f�[�^�̏����o��
	//WriteRender(render, bufferIndex);
    uint particleCount = particleCountBuffer.Load(0);
    if (particleCount > DTid.x)
    {
        uint particleIndex = indexBuffer.Load(DTid.x * 4);
        Particle particle = particleBuffer[particleIndex];
        if (particle.life > 0)
        {
            particle.accel += windDirection * elapsdTime;
            particle.velocity += (normalize(snoise(normalize(particle.accel))) * 15 + particle.accel) * elapsdTime;
            float speed = length(particle.velocity);
            if (speed > maxSpeed)
            {
                float3 vec = normalize(particle.velocity);
                particle.velocity = vec * maxSpeed;
            }
            particle.position += particle.velocity * elapsdTime;
            particle.life -= elapsdTime;
            particleBuffer[particleIndex] = particle;
			//�`��p�f�[�^�̏�����
            ParticleRender render = (ParticleRender) 0;
			//�`��p�f�[�^�̃Z�b�g
            render.position = float4(particle.position, 1.0f);
            render.scale = particle.scale;
            render.color = particle.color;
            render.angle = float3(0, 0, 0);
            render.velocity = particle.velocity * elapsdTime;
			//�`��p�f�[�^�̏����o��
            WriteRender(render, particleIndex*17*4);
			
			            //�����Ă镪�J�E���g�𑝂₷
            uint newAliveCount;
            particleCountBuffer.InterlockedAdd(4, 1, newAliveCount);
           // particleNewIndex[newAliveCount] = index;
            newIndexBuffer.Store(newAliveCount * 4, particleIndex);

        }
        else
        {
            uint deleteAliveCount;
            particleCountBuffer.InterlockedAdd(8, 1, deleteAliveCount);
            deleteIndexBuffer[deleteAliveCount] = particleIndex;
        }
    }
    

}