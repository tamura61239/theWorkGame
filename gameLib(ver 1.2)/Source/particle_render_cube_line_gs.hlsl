#include"particle_render.hlsli"
/*****************************************************************/
//　　ワールド座標系の点からスクリーン座標系のキューブ(の線)に変換する
/*****************************************************************/

[maxvertexcount(30)]
void main(
	point VS_OUT input[1],
	inout LineStream<GS_OUT > output
)
{
	float3 lineBox[8] =
	{
		{0.5,0.5,0.5},
		{-0.5,0.5,0.5},
		{-0.5,-0.5,0.5},
		{0.5,-0.5,0.5},
		{0.5,0.5,-0.5},
		{-0.5,0.5,-0.5},
		{-0.5,-0.5,-0.5},
		{0.5,-0.5,-0.5},
	};
	float3 position = (float3)0;
    int i = 0;
	[unroll]
	for (i = 0; i < 2; i++)
	{
        GS_OUT gout = (GS_OUT)0;
		//
		position = input[0].worldPosition.xyz + mul(lineBox[0 + i * 4] * input[0].scale, input[0].rotateMatrix);
		gout.position = mul(float4(position, 1), view);
		gout.position = mul(gout.position, projection);
		gout.color = input[0].color;
		output.Append(gout);
		//
		position = input[0].worldPosition.xyz + mul(lineBox[1 + i * 4] * input[0].scale, input[0].rotateMatrix);
		gout.position = mul(float4(position, 1), view);
		gout.position = mul(gout.position, projection);
		gout.color = input[0].color;
		output.Append(gout);
		output.RestartStrip();
		//
		position = input[0].worldPosition.xyz + mul(lineBox[1 + i * 4] * input[0].scale, input[0].rotateMatrix);
		gout.position = mul(float4(position, 1), view);
		gout.position = mul(gout.position, projection);
		gout.color = input[0].color;
		output.Append(gout);
		//
		position = input[0].worldPosition.xyz + mul(lineBox[2 + i * 4] * input[0].scale, input[0].rotateMatrix);
		gout.position = mul(float4(position, 1), view);
		gout.position = mul(gout.position, projection);
		gout.color = input[0].color;
		output.Append(gout);
		output.RestartStrip();

		//
		position = input[0].worldPosition.xyz + mul(lineBox[2 + i * 4] * input[0].scale, input[0].rotateMatrix);
		gout.position = mul(float4(position, 1), view);
		gout.position = mul(gout.position, projection);
		gout.color = input[0].color;
		output.Append(gout);
		//
		position = input[0].worldPosition.xyz + mul(lineBox[3 + i * 4] * input[0].scale, input[0].rotateMatrix);
		gout.position = mul(float4(position, 1), view);
		gout.position = mul(gout.position, projection);
		gout.color = input[0].color;
		output.Append(gout);
		output.RestartStrip();
		//
		position = input[0].worldPosition.xyz + mul(lineBox[3 + i * 4] * input[0].scale, input[0].rotateMatrix);
		gout.position = mul(float4(position, 1), view);
		gout.position = mul(gout.position, projection);
		gout.color = input[0].color;
		output.Append(gout);
		//
		position = input[0].worldPosition.xyz + mul(lineBox[0 + i * 4] * input[0].scale, input[0].rotateMatrix);
		gout.position = mul(float4(position, 1), view);
		gout.position = mul(gout.position, projection);
		gout.color = input[0].color;
		output.Append(gout);
		output.RestartStrip();

	}
	[unroll]

	for (i = 0; i < 4; i++)
	{
        GS_OUT gout = (GS_OUT) 0;
		position = input[0].worldPosition.xyz + mul(lineBox[i] * input[0].scale, input[0].rotateMatrix);
		gout.position = mul(float4(position, 1), view);
		gout.position = mul(gout.position, projection);
		gout.color = input[0].color;
		output.Append(gout);

		position = input[0].worldPosition.xyz + mul(lineBox[i + 4] * input[0].scale, input[0].rotateMatrix);
		gout.position = mul(float4(position, 1), view);
		gout.position = mul(gout.position, projection);
		gout.color = input[0].color;
		output.Append(gout);
		output.RestartStrip();
	}

}