struct Particle
{
	float3 position;
	float3 accel;
	float3 velocity;
	float life;
	float4 color;
	float3 scale;
};
//�萔�o�b�t�@�̃f�[�^
cbuffer CbCreate:register(b0)
{
	int startIndex;
	float3 createCentralPosition;
	float randX;
	float3 createArea;
	float4 color;
	float maxLife;
	float scale;
	float2 colorRatio;
	float4 color2;
}
cbuffer CbUpdate : register(b1)
{
	float elapsdTime;
	float3 windDirection;
	float maxSpeed;
	float3 dummy2;
}
//�p�[�e�B�N���̃f�[�^
RWStructuredBuffer<Particle> particleBuffer : register(u0);
//�p�[�e�B�N���̐�
RWByteAddressBuffer particleCountBuffer : register(u1);
//�p�[�e�B�N����index�f�[�^
RWByteAddressBuffer indexBuffer : register(u3);
RWByteAddressBuffer newIndexBuffer : register(u4);
RWStructuredBuffer<uint> deleteIndexBuffer : register(u5);
