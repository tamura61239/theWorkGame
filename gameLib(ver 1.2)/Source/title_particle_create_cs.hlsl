#include"title_psrticle_compute.hlsli"
#include"Lib/Shaders/rand_function.hlsli"
#include"particle_count_buffer.hlsli"

/****************************************************************************/
//�@�@�@�p�[�e�B�N���𐶐�����
/****************************************************************************/
[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	//index�l�̎擾
	uint index = startIndex + DTid.x;
	    //����ł�p�[�e�B�N���̐���1���炷
    uint deadCount;
    particleCountBuffer.InterlockedAdd(4 * 2, -1, deadCount);
    uint newParticleIndex = deleteIndexBuffer[deadCount - 1];

	//�f�[�^�̏�����
	Particle p = (Particle)0;

	//x,y,z�̒l���^���������g���ďo��
	float x = rand_1_normal(float2(index % 591, index % 256), 1) * sphereRatio.x;
	float y = rand_1_normal(float2(index % 777, index % 444), 1) * sphereRatio.y;
	float z = rand_1_normal(float2(index % 321, index % 666), 1) * sphereRatio.z;
	//�o�����l��-1�`1�̊ԂɎ��߂�
	x -= sphereRatio.x;
	y -= sphereRatio.y;
	z -= sphereRatio.z;
	//�v�Z����x,y,z�̒l���g���ď������W���o��
	p.position = normalize(float3(x, y, z)) * rand_1_normal(float2(index % 673, index % 912),0.5f) * leng + startPosition;

	p.scale = float3(1, 1, 1) * (rand_1_normal(float2(index % 345, index % 190), 0.5f))* randScale;
	p.speed = rand_1_normal(float2(index % 666, index % 294), 1)* randSpeed + defSpeed;

	p.life = life;
	p.color = color;
	p.maxLife = p.life;
	p.defVelocity = defVelocity;
	p.defPosition = p.position;
	p.startPosition = p.position;

	p.moveAngleMovement = (rand_1_normal(float2(index % 556, index % 442), 1) - 1)* randMoveAngle;

	p.moveAngleLength = rand_1_normal(float2(index % 123, index % 581), 1) * randMoveLength + defMoveLength;
	
    //���������p�[�e�B�N�����o�b�t�@�ɃZ�b�g����
    particleBuffer[newParticleIndex] = p;
    //�J�E���g�𑝂₷
    uint aliveCount;
    particleCountBuffer.InterlockedAdd(0, 1, aliveCount);
    particleIndexBuffer.Store(aliveCount * 4, newParticleIndex);

}