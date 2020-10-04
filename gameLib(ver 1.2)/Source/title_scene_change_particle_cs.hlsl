#include"title_psrticle_compute.hlsli"
#include"render_particle_cube.hlsli"

[numthreads(100, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	//index�l�̎擾
	uint index = DTid.x + DTid.y + DTid.z;
	uint bufferIndex = index * MAX;
	Particle p;
	//�X�V�f�[�^�̓ǂݍ���
	ReadParticle(p, bufferIndex);
	/*************�X�V***************/

	float3 vec = float3(0, 0, 0) - p.position;
	float l = length(vec);
	vec = normalize(vec);
	float3 vec2 = cross(float3(0, 0, -1), vec);
	p.velocity = (vec2 + vec)*p.speed * elapsdTime;
	p.position += p.velocity;
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