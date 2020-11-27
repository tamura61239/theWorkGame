#include"title_texture_particle.hlsli"
#include"render_particle_cube.hlsli"

[numthreads(100, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	uint index = DTid.x + DTid.y + DTid.z;
	Particle p = particleBuffer[index];
	p.position += p.velocity*p.speed *elapsdTime;
	particleBuffer[index] = p;
	//�`��p�f�[�^�̏�����
	ParticleRender render = (ParticleRender)0;
	//�`��p�f�[�^�̃Z�b�g
	render.position = float4(p.position,1);
	render.scale = scale;
	render.color = p.color;
	render.velocity = p.velocity;
	uint bufferIndex = index * 17 * 4;
	//�`��p�f�[�^�̏����o��
	WriteRender(render, bufferIndex);

}