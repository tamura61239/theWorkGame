#include"title_psrticle_compute.hlsli"
#include"render_particle_cube.hlsli"
#include"particle_count_buffer.hlsli"

/****************************************************************************/
//�@�@�@�p�[�e�B�N�����X�V
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
		if(p.life>0)
        {
			//�X�V
            p.moveAngle += p.moveAngleMovement * elapsdTime;
			
            p.defPosition += p.defVelocity * p.speed * elapsdTime;
            p.life -= elapsdTime;
			
            float3 right = cross(normalize(p.defVelocity), float3(1, 1, 0));
			
            p.moveAngle += p.moveAngleMovement * elapsdTime;
			
            p.position = p.defPosition + right * sin(p.moveAngle) * p.moveAngleLength;
			
            p.life -= elapsdTime;
			
            particleBuffer[index] = p;
            ParticleRender render = (ParticleRender) 0;
			//�`��p�f�[�^�̍X�V
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