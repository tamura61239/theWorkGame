#include"fireworks_particle.hlsli"
#include"rand_function.hlsli"

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	uint index = DTid.x + DTid.y + DTid.z;
	uint count = 0;
    uint totalIndex = 0;
	[unroll]
    for (int i = 0; i < 30;i++)
    {
        //���̃t���[���Ńp�[�e�B�N���𐶐�����G�~�b�^�[����1�G�~�b�^�[������̐������𑫂�
        totalIndex += createData[i].firework.maxCount;
        //����index��totalIndex�Ŋ���
        int anser = (int)(index / totalIndex);
        //�������l��1�ȏ�Ȃ�J�E���g�𑝂₷
        count += saturate(anser);
    }
     Particle p = (Particle) 0;
    //���̖ʏ�̓_���擾
	float angle = (rand_1_normal(float2(index % 28, index % 51), 1) - 1) * 3.14f;
	float z = rand_1_normal(float2(index % 43, index % 12), 1) - 1;
	float x = sqrt(1 - z * z) * cos(angle);
	float y = sqrt(1 - z * z) * sin(angle);
    //�l���Z�b�g����
	p.position = createData[count].position;
    p.velocity = createData[count].firework.speed * float3(x, y, z) * (rand_1_normal(float2(index % 356, index % 263), 0.5f) + 0.4f);
    float velocityLength = length(p.velocity);
    p.startColor = lerp(createData[count].firework.color, createData[count].firework.endColor, step(velocityLength, createData[count].firework.speed * createData[count].firework.parsent));
	p.endColor = createData[count].firework.endColor;
	p.life = 1;
    p.lifeAmoust = 1 / createData[count].firework.maxLife;
	p.scale = createData[count].firework.scale;
	
    p.accel = createData[count].firework.speed * float3(-x * createData[count].firework.parsent, createData[count].firework.gravity, -z * createData[count].firework.parsent);
	p.endTimer = createData[count].firework.endTimer;
    //�ݒ肵���l���p�[�e�B�N�̔z��ɓ����
    particleBuffer[startIndex+index] = p;
}