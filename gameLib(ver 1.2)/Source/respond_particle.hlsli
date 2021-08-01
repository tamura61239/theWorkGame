cbuffer CbBone:register(b0)
{
	row_major float4x4 bone[128];
}
cbuffer CbCreate : register(b1)
{
	uint color;
	float expansionTime;
	float respondTime;
	float expansionspeed;
	float3 respondPosition;
	float centerY;
	float defScale;
}
cbuffer CbUpdate:register(b3)
{
	float elapsdTime;
}

struct Mesh
{
	float3 position;
	float3 normal;
	float2 texcoord;
	float4 boneWeight;
	uint4 boneIndex;
};
struct Particle
{
	float3 start;
	float3 end;
	float respondTime;
	float timer;
	float expansionTime;
	float3 velocity;
	float4 color;
	float scale;

};
//�p�[�e�B�N���̃o�b�t�@�f�[�^
RWStructuredBuffer<Particle> particle : register(u0);
//���f���̃��b�V���f�[�^
StructuredBuffer<Mesh> vertexBuffer : register(t0);
//���f����index�f�[�^
StructuredBuffer<int> indexBuffer : register(t1);

//�����֐�
void CreateParticle(float3 pos, float3 modelCenter, float3 normal, uint index, float time ,float scale, RWStructuredBuffer<Particle> particle, RWByteAddressBuffer countBuffer, RWByteAddressBuffer indexBuffer)
{

    //�p�[�e�B�N���̏����p�����[�^��ݒ�
    Particle p = (Particle) 0;
    p.respondTime = respondTime + time;
    p.expansionTime = expansionTime - time;
    p.start = pos;
    p.end = p.start + (respondPosition - mul(float4(0, 0, 0, 1), bone[0]).xyz);
    p.velocity = normalize(normalize(p.start - modelCenter) + normal) * expansionspeed;
    p.color.r = ((color >> 24) & 0x000000FF) / 255.0f;
    p.color.g = ((color >> 16) & 0x000000FF) / 255.0f;
    p.color.b = ((color >> 8) & 0x000000FF) / 255.0f;
    p.color.a = ((color >> 0) & 0x000000FF) / 255.0f;
    p.scale = scale;
	        //���������p�[�e�B�N�����o�b�t�@�ɃZ�b�g����
    particle[index] = p;
	        //�J�E���g�𑝂₷
    uint aliveCount;
    countBuffer.InterlockedAdd(0, 1, aliveCount);
    indexBuffer.Store(aliveCount * 4, index);

}
//�X�V�Ɏg���C�[�W���O�֐�
float OutCubic(float time, float totaltime, float _max, float _min)
{
    _max -= _min;
    time = time / totaltime - 1.0f;
    return _max * (time * time * time + 1.0f) + _min;
}
float InCubic(float time, float totaltime, float _max, float _min)
{
    _max -= _min;
    time /= totaltime;
    return _max * time * time * time + _min;

}
