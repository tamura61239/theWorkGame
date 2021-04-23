#include"run_particle_cs_function.hlsli"
#include"render_particle_cube.hlsli"
/****************************************************************************/
//�@�@�@�p�[�e�B�N���̍X�V������
/****************************************************************************/

[numthreads(100, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    //�p�[�e�B�N���̍ő吔���擾����
    uint aliveCount = particleCount.Load(0);
    if (aliveCount > DTid.x)
    {//�p�[�e�B�N���̍ő吔��菬�����Ƃ�
        //index�o�b�t�@�ɓo�^���Ă���ԍ��̃p�[�e�B�N���f�[�^���擾����
        uint index;
        index=particleIndex.Load(DTid.x * 4);
        Particle p = particle[index];
        if (p.life > 0)
        {//�������܂�����Ȃ�
            float4 particleColor = (float4) 0;
            //uint�^�ɂ����F�f�[�^��float4�Ŏ擾����
            particleColor.r = ((p.color >> 24) & 0x000000FF) / 255.0f;
            particleColor.g = ((p.color >> 16) & 0x000000FF) / 255.0f;
            particleColor.b = ((p.color >> 8) & 0x000000FF) / 255.0f;
            particleColor.a = ((p.color >> 0) & 0x000000FF) / 255.0f;
            //�X�V
            p.position += p.velocity * elapsdTime;
            p.life -= elapsdTime;
            particleColor.a = p.life;
            //�F�f�[�^��float4����uint�^�ɂ���
            p.color |= ((uint) (particleColor.r * 255) & 0x000000FF) << 24;
            p.color |= ((uint) (particleColor.g * 255) & 0x000000FF) << 16;
            p.color |= ((uint) (particleColor.b * 255) & 0x000000FF) << 8;
            p.color |= ((uint) (particleColor.a * 255) & 0x000000FF) << 0;

            particle[index] = p;
            //�`��p�f�[�^�̍X�V
            ParticleRender render = (ParticleRender) 0;
            render.position = float4(p.position, 1);
            render.scale = p.scale;
            render.color = particleColor;
            render.velocity = p.velocity;
            WriteRender(render, index * 17 * 4);

            //�����Ă镪�J�E���g�𑝂₷
            uint newAliveCount;
            particleCount.InterlockedAdd(4, 1, newAliveCount);
            particleNewIndex.Store(newAliveCount * 4, index);

        }
        else
        {
            //����ł镪�̃J�E���g�𑝂₷
            uint deleteCount;
            particleCount.InterlockedAdd(4 * 2, 1, deleteCount);
            deleteIndex[deleteCount] = index;

        }

    }

}
