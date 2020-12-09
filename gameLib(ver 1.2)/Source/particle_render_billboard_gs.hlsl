#include"particle_render.hlsli"

[maxvertexcount(4)]
void main(
	point VS_OUT input[1],
	inout TriangleStream<GS_OUT > output
)
{

	float4 texPosition[4] =
	{
		{ -0.5f,0.5f,0,0 },
		{ 0.5f,0.5f,0,0 },
		{ -0.5f,-0.5f,0,0 },
		{ 0.5f,-0.5f,0,0 },
	};
	float2 texcoord[4] =
	{
		{0.0f,0.0f},
		{1.0f,0.0f},
		{0.0f,1.0f},
		{1.0f,1.0f},
	};
	[unroll]
	for (int i = 0; i < 4; i++)
	{
		GS_OUT gout;
		gout.position = mul(mul(float4(input[0].worldPosition, 1), view) + texPosition[i] * input[0].scale.x, projection);
		gout.texcoord = texcoord[i];
		gout.color = input[0].color;
		output.Append(gout);
	}
	output.RestartStrip();

}