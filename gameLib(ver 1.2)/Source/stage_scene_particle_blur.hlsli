struct VS_OUT
{
	float4 position: POSITION;
	float3 scale:SCALE;
	float3 velocity:VELOCITY;
	float3 angle:ANGLE;
};
struct GS_OUT
{
	float4 position:SV_POSITION;
	float4 velocity:VELOCITY;
};
cbuffer CbScene:register(b0)
{
	row_major float4x4 view;
	row_major float4x4 projection;
}
cbuffer Cb : register(b1)
{
	float elapsdTime;
	float3 angleMovement;
}
cbuffer CbBeforeScene : register(b5)
{
	row_major float4x4 beforeView;
	row_major float4x4 beforeProjection;
}

float3x3 MakeRotation(float3 axis, float angle)
{
	float cs = cos(angle);
	float sn = sin(angle);
	float oneMinusCos = 1.0 - cs;
	float3 squared = axis * axis;
	float xym = axis.x * axis.y * oneMinusCos;
	float xzm = axis.x * axis.z * oneMinusCos;
	float yzm = axis.y * axis.z * oneMinusCos;
	float3 xyzSin = axis * sn;
	return float3x3(
		squared.x * oneMinusCos + cs, xym - xyzSin.z, xzm + xyzSin.y,
		xym + xyzSin.z, squared.y * oneMinusCos + cs, yzm - xyzSin.x,
		xzm - xyzSin.y, yzm + xyzSin.x, squared.z * oneMinusCos + cs
		);
}