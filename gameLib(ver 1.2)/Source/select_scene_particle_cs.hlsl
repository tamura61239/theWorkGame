#include"select_scene_particle.hlsli"
#include"render_particle_cube.hlsli"
#include"curl_noise.hlsli"
#include"matrix_calculation.hlsli"
#include"particle_count_buffer.hlsli"

/****************************************************************************/
//�@�@�@�p�[�e�B�N���̍X�V������
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
		
            //�`��p�f�[�^�̍X�V
            render.position = particle.position;
            render.angle = particle.angle;
            render.color = particle.color;
            render.velocity = particle.velocity;
            render.scale = float3(1, 1, 1) * 0.5f;
            WriteRender(render, particleIndex * 17 * 4);
            //�����Ă镪�J�E���g�𑝂₷
            uint newParticleCount;
            particleCountBuffer.InterlockedAdd(4, 1, newParticleCount);
            newIndexBuffer.Store(newParticleCount * 4, particleIndex);

        }
        else
        {
            //����ł镪�̃J�E���g�𑝂₷
            uint deleteParticleCount;
            particleCountBuffer.InterlockedAdd(8, 1, deleteParticleCount);
            deleteIndexBuffer[deleteParticleCount] = particleIndex;
        }

    }
}