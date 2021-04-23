#include"title_psrticle_compute.hlsli"
#include"render_particle_cube.hlsli"
/****************************************************************************/
//�@�@�@�p�[�e�B�N���������֋z�����܂�Ă�������
/****************************************************************************/

[numthreads(100, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	//index�l�̎擾
	uint index = DTid.x + DTid.y + DTid.z;
	uint bufferIndex = index * MAX;
	Particle p;
	//�X�V�f�[�^�̓ǂݍ���
	ReadParticle(p, bufferIndex);
    //�X�V
	float3 vec = float3(0, 0, 0) - p.position;
	float l = length(vec);
	vec = normalize(vec);
	float3 vec2 = cross(float3(0, 0, -1), vec);
	
	p.velocity = lerp((vec2 + vec) * p.speed * 10.f * elapsdTime, (vec2 + vec) * p.speed * l * elapsdTime, step(l,10));
	p.position += p.velocity;
	l = length(float3(0, 0, 0) - p.position);
	if (abs(l) <= 2.f)
	{//���ȉ��̋����ɂȂ��������
		p.color = float4(0, 0, 0, 0);
		p.position = float3(0, 0, 0);
		p.scale = float3(0, 0, 0);
		p.life = 0;
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