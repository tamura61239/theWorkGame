struct Particle
{
	float3 position;
	float3 velocity;
	float4 color;
	float speed;
};
//�萔�o�b�t�@�̃f�[�^
cbuffer CbCreate:register(b0)
{
	float2 uv;
	float2 uvSize;
	row_major float4x4 world;
	float screenSplit;
	int startIndex;
}
cbuffer CbUpdate : register(b1)
{
	float elapsdTime;
	float scale;
	float speed;
	float2 spiralRatio;

}
//�e�N�X�`���f�[�^
Texture2D<float4>scene : register(t0);
//�p�[�e�B�N���f�[�^
RWStructuredBuffer< Particle>particleBuffer:register(u0);