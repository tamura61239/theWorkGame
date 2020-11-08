#include"particle_render.hlsli"

[maxvertexcount(36)]
void main(
	point VS_OUT input[1],
	inout TriangleStream<GS_OUT > output
)
{
	// ���_�f�[�^�̒�`
// 0           1
// +-----------+
// |           |
// |           |
// +-----------+
// 2           3

	float3 triangleBox[8] =
	{
		//��O
		{-0.5,0.5,-0.5},
		{0.5,0.5,-0.5},
		{-0.5,-0.5,-0.5},
		{0.5,-0.5,-0.5},
		//��
		{-0.5,0.5,0.5},
		{0.5,0.5,0.5},
		{-0.5,-0.5,0.5},
		{0.5,-0.5,0.5},

	};
	uint index[] =
	{
		//��O
		0,1,2,
		2,1,3,
		//��
		5,4,7,
		7,4,6,
		//�E
		1,5,3,
		3,5,7,
		//��
		0,2,4,
		4,2,6,
		//��
		4,5,0,
		0,5,1,
		//��
		6,2,7,
		7,2,3,
	};

	float3 position = (float3)0;
	[unroll]
	for (int i = 0; i < 12; i++)
	{
		GS_OUT gout;
		position = input[0].worldPosition.xyz + mul(triangleBox[index[i * 3]] * input[0].scale, input[0].rotateMatrix);
		gout.position = mul(float4(position, 1), view);
		gout.position = mul(gout.position, projection);
		gout.color = input[0].color;
		output.Append(gout);

		position = input[0].worldPosition.xyz + mul(triangleBox[index[i * 3 + 1]] * input[0].scale, input[0].rotateMatrix);
		gout.position = mul(float4(position, 1), view);
		gout.position = mul(gout.position, projection);
		gout.color = input[0].color;
		output.Append(gout);

		position = input[0].worldPosition.xyz + mul(triangleBox[index[i * 3 + 2]] * input[0].scale, input[0].rotateMatrix);
		gout.position = mul(float4(position, 1), view);
		gout.position = mul(gout.position, projection);
		gout.color = input[0].color;
		output.Append(gout);
		output.RestartStrip();
	}
}