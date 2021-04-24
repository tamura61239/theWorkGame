#include"fireworks_particle.hlsli"
#include"render_particle_cube.hlsli"
#include"particle_count_buffer.hlsli"
/****************************************************************************/
//�@�@�@�ԉ΂̃p�[�e�B�N���̍X�V
/****************************************************************************/

[numthreads(100, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	    //�p�[�e�B�N���̍ő吔���擾����
    uint aliveCount = particleCountBuffer.Load(0);
    if (aliveCount > DTid.x)
    { //�p�[�e�B�N���̍ő吔��菬�����Ƃ�
        //index�o�b�t�@�ɓo�^���Ă���ԍ��̃p�[�e�B�N���f�[�^���擾����
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
            //�`��p�f�[�^�̏�����
            ParticleRender render = (ParticleRender) 0;
            //�`��p�f�[�^�̃Z�b�g
            render.position = float4(p.position, color.a);
            render.scale = p.scale;
            render.color = color;
            render.velocity = p.velocity * elapsdTime;
            uint bufferIndex = index * 17 * 4;
            //�`��p�f�[�^�̏����o��
            WriteRender(render, bufferIndex);
            
            //�����Ă镪�J�E���g�𑝂₷
            uint newAliveCount;
            particleCountBuffer.InterlockedAdd(4, 1, newAliveCount);
            newIndexBuffer.Store(newAliveCount * 4, index);

        }
        else
        {
            //����ł镪�̃J�E���g�𑝂₷
            uint deleteCount;
            particleCountBuffer.InterlockedAdd(4 * 2, 1, deleteCount);
            deleteIndexBuffer[deleteCount] = index;

        }
    }
}