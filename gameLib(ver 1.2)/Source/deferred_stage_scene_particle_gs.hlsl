#include"deferred_stage_scene_particle.hlsli"

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
	float3x3 rotateMatrix = mul(MakeRotation(float3(1, 0, 0), input[0].angle.x), mul(MakeRotation(float3(0, 1, 0), input[0].angle.y), MakeRotation(float3(0, 0, 1), input[0].angle.z)));
	float3 beforeAngle = input[0].angle - angleMovement * elapsdTime;
	float3x3 beforeRotateMatrix = mul(MakeRotation(float3(1, 0, 0), beforeAngle.x), mul(MakeRotation(float3(0, 1, 0), beforeAngle.y), MakeRotation(float3(0, 0, 1), beforeAngle.z)));

	float3 position = (float3)0, beforePosition = (float3)0;
	float4 beforeVPPosition = (float4)0;
	for (int i = 0; i < 2; i++)
	{
		GS_OUT gout;
		//現フレームと前のフレームのpositionの取得
		position = input[0].position.xyz + mul(lineBox[0 + i * 4] * input[0].scale, rotateMatrix);
		beforePosition = input[0].position.xyz - input[0].velocity * elapsdTime + mul(lineBox[0 + i * 4] * input[0].scale, beforeRotateMatrix);
		//現フレームのビュープロジェクション変換
		gout.position = mul(float4(position, 1), view);
		gout.position = mul(gout.position, projection);
		//前のフレームのビュープロジェクション変換
		beforeVPPosition = mul(float4(beforePosition, 1), beforeView);
		beforeVPPosition = mul(beforeVPPosition, beforeProjection);
		//現フレーム-前のフレームから速度を出す
		gout.velocity = beforeVPPosition.xyz - gout.position.xyz;

		gout.color = input[0].color;
		output.Append(gout);
		//
		position = input[0].position.xyz + mul(lineBox[1 + i * 4] * input[0].scale, rotateMatrix);
		beforePosition = input[0].position.xyz - input[0].velocity * elapsdTime + mul(lineBox[0 + i * 4] * input[0].scale, beforeRotateMatrix);
		gout.position = mul(float4(position, 1), view);
		gout.position = mul(gout.position, projection);

		beforeVPPosition = mul(float4(beforePosition, 1), beforeView);
		beforeVPPosition = mul(beforeVPPosition, beforeProjection);

		gout.velocity = beforeVPPosition.xyz - gout.position.xyz;

		gout.color = input[0].color;
		output.Append(gout);
		output.RestartStrip();
		//
		position = input[0].position.xyz + mul(lineBox[1 + i * 4] * input[0].scale, rotateMatrix);
		beforePosition = input[0].position.xyz - input[0].velocity * elapsdTime + mul(lineBox[0 + i * 4] * input[0].scale, beforeRotateMatrix);
		gout.position = mul(float4(position, 1), view);
		gout.position = mul(gout.position, projection);

		beforeVPPosition = mul(float4(beforePosition, 1), beforeView);
		beforeVPPosition = mul(beforeVPPosition, beforeProjection);

		gout.velocity = beforeVPPosition.xyz - gout.position.xyz;
		gout.color = input[0].color;
		output.Append(gout);
		//
		position = input[0].position.xyz + mul(lineBox[2 + i * 4] * input[0].scale, rotateMatrix);
		beforePosition = input[0].position.xyz - input[0].velocity * elapsdTime + mul(lineBox[0 + i * 4] * input[0].scale, beforeRotateMatrix);
		gout.position = mul(float4(position, 1), view);
		gout.position = mul(gout.position, projection);

		beforeVPPosition = mul(float4(beforePosition, 1), beforeView);
		beforeVPPosition = mul(beforeVPPosition, beforeProjection);

		gout.velocity = beforeVPPosition.xyz - gout.position.xyz;
		gout.color = input[0].color;
		output.Append(gout);
		output.RestartStrip();

		//
		position = input[0].position.xyz + mul(lineBox[2 + i * 4] * input[0].scale, rotateMatrix);
		beforePosition = input[0].position.xyz - input[0].velocity * elapsdTime + mul(lineBox[0 + i * 4] * input[0].scale, beforeRotateMatrix);
		gout.position = mul(float4(position, 1), view);
		gout.position = mul(gout.position, projection);

		beforeVPPosition = mul(float4(beforePosition, 1), beforeView);
		beforeVPPosition = mul(beforeVPPosition, beforeProjection);

		gout.velocity = beforeVPPosition.xyz - gout.position.xyz;
		gout.color = input[0].color;
		output.Append(gout);
		//
		position = input[0].position.xyz + mul(lineBox[3 + i * 4] * input[0].scale, rotateMatrix);
		beforePosition = input[0].position.xyz - input[0].velocity * elapsdTime + mul(lineBox[0 + i * 4] * input[0].scale, beforeRotateMatrix);
		gout.position = mul(float4(position, 1), view);
		gout.position = mul(gout.position, projection);

		beforeVPPosition = mul(float4(beforePosition, 1), beforeView);
		beforeVPPosition = mul(beforeVPPosition, beforeProjection);

		gout.velocity = beforeVPPosition.xyz - gout.position.xyz;
		gout.color = input[0].color;
		output.Append(gout);
		output.RestartStrip();
		//
		position = input[0].position.xyz + mul(lineBox[3 + i * 4] * input[0].scale, rotateMatrix);
		beforePosition = input[0].position.xyz - input[0].velocity * elapsdTime + mul(lineBox[0 + i * 4] * input[0].scale, beforeRotateMatrix);
		gout.position = mul(float4(position, 1), view);
		gout.position = mul(gout.position, projection);

		beforeVPPosition = mul(float4(beforePosition, 1), beforeView);
		beforeVPPosition = mul(beforeVPPosition, beforeProjection);

		gout.velocity = beforeVPPosition.xyz - gout.position.xyz;
		gout.color = input[0].color;
		output.Append(gout);
		//
		position = input[0].position.xyz + mul(lineBox[0 + i * 4] * input[0].scale, rotateMatrix);
		beforePosition = input[0].position.xyz - input[0].velocity * elapsdTime + mul(lineBox[0 + i * 4] * input[0].scale, beforeRotateMatrix);
		gout.position = mul(float4(position, 1), view);
		gout.position = mul(gout.position, projection);

		beforeVPPosition = mul(float4(beforePosition, 1), beforeView);
		beforeVPPosition = mul(beforeVPPosition, beforeProjection);

		gout.velocity = beforeVPPosition.xyz - gout.position.xyz;
		gout.color = input[0].color;
		output.Append(gout);
		output.RestartStrip();

	}

	for (int i = 0; i < 4; i++)
	{
		GS_OUT gout;
		position = input[0].position.xyz + mul(lineBox[i] * input[0].scale, rotateMatrix);
		beforePosition = input[0].position.xyz - input[0].velocity * elapsdTime + mul(lineBox[i] * input[0].scale, beforeRotateMatrix);

		gout.position = mul(float4(position, 1), view);
		gout.position = mul(gout.position, projection);

		beforeVPPosition = mul(float4(beforePosition, 1), beforeView);
		beforeVPPosition = mul(beforeVPPosition, beforeProjection);

		gout.velocity = beforeVPPosition.xyz - gout.position.xyz;

		gout.color = input[0].color;
		output.Append(gout);

		position = input[0].position.xyz + mul(lineBox[i + 4] * input[0].scale, rotateMatrix);
		beforePosition = input[0].position.xyz - input[0].velocity * elapsdTime + mul(lineBox[i + 4] * input[0].scale, beforeRotateMatrix);

		gout.position = mul(float4(position, 1), view);
		gout.position = mul(gout.position, projection);

		beforeVPPosition = mul(float4(beforePosition, 1), beforeView);
		beforeVPPosition = mul(beforeVPPosition, beforeProjection);

		gout.velocity = beforeVPPosition.xyz - gout.position.xyz;

		gout.color = input[0].color;
		output.Append(gout);
		output.RestartStrip();
	}

}