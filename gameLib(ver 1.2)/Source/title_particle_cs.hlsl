#include"title_psrticle_compute.hlsli"
#include"render_particle_cube.hlsli"
#include"curl_noise.hlsli"
#include"matrix_calculation.hlsli"

[numthreads(100, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	//index�l�̎擾
	uint index = DTid.x + DTid.y + DTid.z;
	uint bufferIndex = index * MAX;
	Particle p;
	//�X�V�f�[�^�̓ǂݍ���
	ReadParticle(p, bufferIndex);

	/*************�X�V***************/
	p.moveAngle += p.moveAngleMovement * elapsdTime;

	p.defPosition += p.defVelocity * p.speed * elapsdTime;
	p.life -= elapsdTime;

	float3 right = cross(normalize(p.defVelocity), float3(1, 1, 0));

	p.moveAngle += p.moveAngleMovement * elapsdTime;

	p.position = p.defPosition + right * sin(p.moveAngle) * p.moveAngleLength;

	p.life -= elapsdTime;
	//life��0�ȉ��̎�
	if (p.life <= 0)
	{
		p.color = float4(0, 0, 0, 0);
		p.position = float3(0, 0, 0);
		p.scale = float3(0, 0, 0);
	}
	//�X�V�f�[�^�̏����o��
	WriteParticle(p, bufferIndex);

	//�`��p�f�[�^�̏�����
	ParticleRender render = (ParticleRender)0;
	//�`��p�f�[�^�̃Z�b�g
	render.position = float4(p.position, 1.0f);
	render.scale = p.scale;
	render.color = p.color;
	render.angle = p.angle;
	render.velocity = p.velocity;
	bufferIndex = index * 17 * 4;
	//�`��p�f�[�^�̏����o��
	WriteRender(render, bufferIndex);
}