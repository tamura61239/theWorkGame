#include"title_psrticle_compute.hlsli"
#include"render_particle_cube.hlsli"
#include"particle_count_buffer.hlsli"

/****************************************************************************/
//�@�@�@�p�[�e�B�N���������֋z�����܂�Ă�������
/****************************************************************************/

[numthreads(100, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint aliveCount = particleCountBuffer.Load(0);
    if (aliveCount > DTid.x)
    { //�p�[�e�B�N���̍ő吔��菬�����Ƃ�
        //index�o�b�t�@�ɓo�^���Ă���ԍ��̃p�[�e�B�N���f�[�^���擾����
        uint index;
        index = particleIndexBuffer.Load(DTid.x * 4);
        Particle p = particleBuffer[index];
        float l = length(float3(0, 0, 0) - p.position);
        if (abs(l) > 2.f)
        {
			//�X�V
            float3 vec = float3(0, 0, 0) - p.position;
            l = length(vec);
            vec = normalize(vec);
            float3 vec2 = cross(float3(0, 0, -1), vec);
			
            p.velocity = lerp((vec2 + vec) * p.speed * 10.f * elapsdTime, (vec2 + vec) * p.speed * l * elapsdTime, step(l, 10));
            p.position += p.velocity;
            particleBuffer[index] = p;
			//�`��p�f�[�^�̏�����
            ParticleRender render = (ParticleRender) 0;
			//�`��p�f�[�^�̃Z�b�g
            render.position = float4(p.position, 1.0f);
            render.scale = p.scale;
            render.color = p.color;
            render.angle = p.angle;
            render.velocity = p.velocity;
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