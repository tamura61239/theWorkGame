#include"fireworks_particle.hlsli"
#include"rand_function.hlsli"


[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint index = DTid.x + DTid.y + DTid.z;
    uint count = 0;
    uint emitorNumber = index / 100;
    uint totalIndex = 0;
    [unroll]
    for (int i = 0; i < 30; i++)
    {
        //���̃t���[���Ńp�[�e�B�N���𐶐�����G�~�b�^�[����1�G�~�b�^�[������̐������𑫂�
        totalIndex += createData[i].firework.maxCount;
        //����index��totalIndex�Ŋ���
        int anser = (int) (index / totalIndex);
        //�������l��1�ȏ�Ȃ�J�E���g�𑝂₷
        emitorNumber +=  saturate(anser);
        count = lerp(count, totalIndex-index, saturate(anser+1));
    }

    Particle p = (Particle) 0;
    p.position = createData[emitorNumber].position - createData[emitorNumber].velocity * (count / createData[emitorNumber].firework.maxCount);
    p.startColor = float4(1,1,1,1);
    p.endTimer = createData[emitorNumber].firework.endTimer;
    p.scale = createData[emitorNumber].firework.scale;

    particleBuffer[startIndex + index] = p;

}