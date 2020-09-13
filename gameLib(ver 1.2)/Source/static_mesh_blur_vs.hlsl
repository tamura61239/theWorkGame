#include "static_mesh_blur.hlsli"

VS_OUT main(float4 position : POSITION, float3 normal : NORMAL, float2 texcoord : TEXCOORD/*UNIT.13*/)
{
	VS_OUT vout;
	//���̃t���[���̍��W
	float4 nowPosition = mul(position, world);
	nowPosition = mul(nowPosition, view);
	nowPosition = mul(nowPosition, projection);
	//�O�̃t���[���̍��W
	float4 beforePosition = mul(position, beforeWorld);
	beforePosition = mul(beforePosition, beforeView);
	beforePosition = mul(beforePosition, beforeProjection);
	//�@��
	float3 N = normalize(mul(normal, /*(float3x3)world*/mul((float3x3)world, (float3x3)mul(view, projection))));
	//�ړ��x�N�g��
	float3 vec = nowPosition.xyz - beforePosition.xyz;
	//�O�̃t���[���ƍ��̃t���[���̓���
	float d = dot(normalize(vec), normalize(N));
	if (d < 0.0f)vout.position = beforePosition;
	else vout.position = nowPosition;
	vout.velocity.xy = ((nowPosition.xy/ nowPosition.z) - (beforePosition.xy/ beforePosition.z))*0.5f;
	vout.velocity.y *= -1.0f;
	vout.velocity.z = vout.position.z;
	vout.velocity.w = vout.position.w;
	return vout;
}
