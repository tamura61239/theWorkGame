#include"stage_scene_particle.hlsli"
#include"curl_noise.hlsli"
#include"render_particle_cube.hlsli"
#include"particle_count_buffer.hlsli"

/****************************************************************************/
//�@�@�@�p�[�e�B�N�����X�V����
/****************************************************************************/

[numthreads(100, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    //�p�[�e�B�N���̐����擾
    uint particleCount = particleCountBuffer.Load(0);
    if (particleCount > DTid.x)
    { //�p�[�e�B�N���̍ő吔��菬�����Ƃ�
        //index�o�b�t�@�ɓo�^���Ă���ԍ��̃p�[�e�B�N���f�[�^���擾����
        uint particleIndex = particleIndexBuffer.Load(DTid.x * 4);
        Particle particle = particleBuffer[particleIndex];
        if (particle.life > 0)
        { //�������܂�����Ȃ�
            //�X�V
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
            //�`��p�f�[�^�̍X�V
            render.position = float4(particle.position, 1.0f);
            render.scale = particle.scale;
            render.color = particle.color;
            render.angle = float3(0, 0, 0);
            render.velocity = particle.velocity * elapsdTime;
            WriteRender(render, particleIndex*17*4);
			
            //�����Ă镪�J�E���g�𑝂₷
            uint newAliveCount;
            particleCountBuffer.InterlockedAdd(4, 1, newAliveCount);
            newIndexBuffer.Store(newAliveCount * 4, particleIndex);

        }
        else
        {
            //����ł镪�̃J�E���g�𑝂₷
            uint deleteAliveCount;
            particleCountBuffer.InterlockedAdd(8, 1, deleteAliveCount);
            deleteIndexBuffer[deleteAliveCount] = particleIndex;
        }
    }
    

}