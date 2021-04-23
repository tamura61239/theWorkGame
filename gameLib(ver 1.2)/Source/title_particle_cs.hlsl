#include"title_psrticle_compute.hlsli"
#include"render_particle_cube.hlsli"
#include"curl_noise.hlsli"
#include"matrix_calculation.hlsli"
/****************************************************************************/
//�@�@�@�p�[�e�B�N�����X�V
/****************************************************************************/

[numthreads(100, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	//index�l�̎擾
	uint index = DTid.x + DTid.y + DTid.z;
	uint bufferIndex = index * MAX;
	Particle p;
	//�X�V�f�[�^�̓ǂݍ���
	ReadParticle(p, bufferIndex);

	//�X�V
	p.moveAngle += p.moveAngleMovement * elapsdTime;

	p.defPosition += p.defVelocity * p.speed * elapsdTime;
	p.life -= elapsdTime;

	float3 right = cross(normalize(p.defVelocity), float3(1, 1, 0));

	p.moveAngle += p.moveAngleMovement * elapsdTime;

	p.position = p.defPosition + right * sin(p.moveAngle) * p.moveAngleLength;

	p.life -= elapsdTime;
	if (p.life <= 0)
	{//�������s���������
		p.color = float4(0, 0, 0, 0);
		p.position = float3(0, 0, 0);
		p.scale = float3(0, 0, 0);
	}
	//�X�V�f�[�^�̏����o��
	WriteParticle(p, bufferIndex);

	ParticleRender render = (ParticleRender)0;
    //�`��p�f�[�^�̍X�V
	render.position = float4(p.position, 1.0f);
	render.scale = p.scale;
	render.color = p.color;
	render.angle = p.angle;
	render.velocity = p.velocity;
	bufferIndex = index * 17 * 4;
	WriteRender(render, bufferIndex);
}