#include"stage_scene_particle_blur.hlsli"

[maxvertexcount(36)]
//void main(
//	point VS_OUT input[1],
//	inout LineStream<GS_OUT > output
//)
//{
//	float3 lineBox[8] =
//	{
//		{0.5,0.5,0.5},
//		{-0.5,0.5,0.5},
//		{-0.5,-0.5,0.5},
//		{0.5,-0.5,0.5},
//		{-0.5,0.5,-0.5},
//		{0.5,0.5,-0.5},
//		{0.5,-0.5,-0.5},
//		{-0.5,-0.5,-0.5},
//	};
//	float3x3 rotateMatrix = mul(MakeRotation(float3(1, 0, 0), input[0].angle.x), mul(MakeRotation(float3(0, 1, 0), input[0].angle.y), MakeRotation(float3(0, 0, 1), input[0].angle.z)));
//	float3 beforeAngle = input[0].angle - angleMovement * elapsdTime;
//	float3x3 beforeRotateMatrix = mul(MakeRotation(float3(1, 0, 0), beforeAngle.x), mul(MakeRotation(float3(0, 1, 0), beforeAngle.y), MakeRotation(float3(0, 0, 1), beforeAngle.z)));
//	
//	float3 position = (float3)0, beforePosition = (float3)0;
//	float3 normal = (float3)0, velocity = (float3)0;
//	float4 beforeVPPosition = (float4)0, VPPosition = (float4)0;
//	float judge = 0;
//	int i = 0;
//	for (i = 0; i < 2; i++)
//	{
//		GS_OUT gout;
//		//���t���[���̖@�����Z�o
//		normal = normalize(cross(lineBox[1 + i * 4] - lineBox[0 + i * 4], lineBox[3 + i * 4] - lineBox[0 + i * 4]));
//		/*************Line**************/
//		//���t���[���ƑO�̃t���[����position�̎擾
//		position = input[0].position.xyz + mul(lineBox[0 + i * 4] * input[0].scale, rotateMatrix);
//		beforePosition = input[0].position.xyz - input[0].velocity * elapsdTime + mul(lineBox[0 + i * 4] * input[0].scale, beforeRotateMatrix);
//		//���t���[���̃r���[�v���W�F�N�V�����ϊ�
//		VPPosition = mul(float4(position, 1), view);
//		VPPosition = mul(VPPosition, projection);
//		//�O�̃t���[���̃r���[�v���W�F�N�V�����ϊ�
//		beforeVPPosition = mul(float4(beforePosition, 1), beforeView);
//		beforeVPPosition = mul(beforeVPPosition, beforeProjection);
//		//���_�̈ړ��x�N�g�����o��
//		velocity = VPPosition.xyz - beforeVPPosition.xyz;
//		//�@����velocity�̓��ς���O�̃t���[���̕����Ɉ������΂������߂�
//		judge = step(0, dot(normalize(velocity), normal));
//		gout.position = lerp(beforeVPPosition, VPPosition, judge);
//		//���x�x�N�g���̌v�Z
//		gout.velocity.xy = (VPPosition.xy / VPPosition.w - beforeVPPosition.xy / beforeVPPosition.w) * 0.5f;
//		gout.velocity.y *= -1.0f;
//		gout.velocity.zw = gout.position.zw;
//		output.Append(gout);
//		//���t���[���ƑO�̃t���[����position�̎擾
//		position = input[0].position.xyz + mul(lineBox[1 + i * 4] * input[0].scale, rotateMatrix);
//		beforePosition = input[0].position.xyz - input[0].velocity * elapsdTime + mul(lineBox[1 + i * 4] * input[0].scale, beforeRotateMatrix);
//		//���t���[���̃r���[�v���W�F�N�V�����ϊ�
//		VPPosition = mul(float4(position, 1), view);
//		VPPosition = mul(VPPosition, projection);
//		//�O�̃t���[���̃r���[�v���W�F�N�V�����ϊ�
//		beforeVPPosition = mul(float4(beforePosition, 1), beforeView);
//		beforeVPPosition = mul(beforeVPPosition, beforeProjection);
//		//���_�̈ړ��x�N�g�����o��
//		velocity = VPPosition.xyz - beforeVPPosition.xyz;
//		//�@����velocity�̓��ς���O�̃t���[���̕����Ɉ������΂������߂�
//		judge = step(0, dot(normalize(velocity), normal));
//		gout.position = lerp(beforeVPPosition, VPPosition, judge);
//		//���x�x�N�g���̌v�Z
//		gout.velocity.xy = (VPPosition.xy / VPPosition.w - beforeVPPosition.xy / beforeVPPosition.w) * 0.5f;
//		gout.velocity.y *= -1.0f;
//		gout.velocity.zw = gout.position.zw;
//		output.Append(gout);
//		output.RestartStrip();
//
//		/*************Line**************/
//        //���t���[���ƑO�̃t���[����position�̎擾
//		position = input[0].position.xyz + mul(lineBox[0 + i * 4] * input[0].scale, rotateMatrix);
//		beforePosition = input[0].position.xyz - input[0].velocity * elapsdTime + mul(lineBox[0 + i * 4] * input[0].scale, beforeRotateMatrix);
//		//���t���[���̃r���[�v���W�F�N�V�����ϊ�
//		VPPosition = mul(float4(position, 1), view);
//		VPPosition = mul(VPPosition, projection);
//		//�O�̃t���[���̃r���[�v���W�F�N�V�����ϊ�
//		beforeVPPosition = mul(float4(beforePosition, 1), beforeView);
//		beforeVPPosition = mul(beforeVPPosition, beforeProjection);
//		//���_�̈ړ��x�N�g�����o��
//		velocity = VPPosition.xyz - beforeVPPosition.xyz;
//		//�@����velocity�̓��ς���O�̃t���[���̕����Ɉ������΂������߂�
//		judge = step(0, dot(normalize(velocity), normal));
//		gout.position = lerp(beforeVPPosition, VPPosition, judge);
//		//���x�x�N�g���̌v�Z
//		gout.velocity.xy = (VPPosition.xy / VPPosition.w - beforeVPPosition.xy / beforeVPPosition.w) * 0.5f;
//		gout.velocity.y *= -1.0f;
//		gout.velocity.zw = gout.position.zw;
//		output.Append(gout);
//		//���t���[���ƑO�̃t���[����position�̎擾
//		position = input[0].position.xyz + mul(lineBox[3 + i * 4] * input[0].scale, rotateMatrix);
//		beforePosition = input[0].position.xyz - input[0].velocity * elapsdTime + mul(lineBox[3 + i * 4] * input[0].scale, beforeRotateMatrix);
//		//���t���[���̃r���[�v���W�F�N�V�����ϊ�
//		VPPosition = mul(float4(position, 1), view);
//		VPPosition = mul(VPPosition, projection);
//		//�O�̃t���[���̃r���[�v���W�F�N�V�����ϊ�
//		beforeVPPosition = mul(float4(beforePosition, 1), beforeView);
//		beforeVPPosition = mul(beforeVPPosition, beforeProjection);
//		//���_�̈ړ��x�N�g�����o��
//		velocity = VPPosition.xyz - beforeVPPosition.xyz;
//		//�@����velocity�̓��ς���O�̃t���[���̕����Ɉ������΂������߂�
//		judge = step(0, dot(normalize(velocity), normal));
//		gout.position = lerp(beforeVPPosition, VPPosition, judge);
//		//���x�x�N�g���̌v�Z
//		gout.velocity.xy = (VPPosition.xy / VPPosition.w - beforeVPPosition.xy / beforeVPPosition.w) * 0.5f;
//		gout.velocity.y *= -1.0f;
//		gout.velocity.zw = gout.position.zw;
//		output.Append(gout);
//		output.RestartStrip();
//
//		//���t���[���̖@�����Z�o
//		normal = normalize(cross(lineBox[1 + i * 4] - lineBox[2 + i * 4], lineBox[3 + i * 4] - lineBox[2 + i * 4]));
//		/*************Line**************/
//		//���t���[���ƑO�̃t���[����position�̎擾
//		position = input[0].position.xyz + mul(lineBox[2 + i * 4] * input[0].scale, rotateMatrix);
//		beforePosition = input[0].position.xyz - input[0].velocity * elapsdTime + mul(lineBox[2 + i * 4] * input[0].scale, beforeRotateMatrix);
//		//���t���[���̃r���[�v���W�F�N�V�����ϊ�
//		VPPosition = mul(float4(position, 1), view);
//		VPPosition = mul(VPPosition, projection);
//		//�O�̃t���[���̃r���[�v���W�F�N�V�����ϊ�
//		beforeVPPosition = mul(float4(beforePosition, 1), beforeView);
//		beforeVPPosition = mul(beforeVPPosition, beforeProjection);
//		//���_�̈ړ��x�N�g�����o��
//		velocity = VPPosition.xyz - beforeVPPosition.xyz;
//		//�@����velocity�̓��ς���O�̃t���[���̕����Ɉ������΂������߂�
//		judge = step(0, dot(normalize(velocity), normal));
//		gout.position = lerp(beforeVPPosition, VPPosition, judge);
//		//���x�x�N�g���̌v�Z
//		gout.velocity.xy = (VPPosition.xy / VPPosition.w - beforeVPPosition.xy / beforeVPPosition.w) * 0.5f;
//		gout.velocity.y *= -1.0f;
//		gout.velocity.zw = gout.position.zw;
//		output.Append(gout);
//		//���t���[���ƑO�̃t���[����position�̎擾
//		position = input[0].position.xyz + mul(lineBox[1 + i * 4] * input[0].scale, rotateMatrix);
//		beforePosition = input[0].position.xyz - input[0].velocity * elapsdTime + mul(lineBox[1 + i * 4] * input[0].scale, beforeRotateMatrix);
//		//���t���[���̃r���[�v���W�F�N�V�����ϊ�
//		VPPosition = mul(float4(position, 1), view);
//		VPPosition = mul(VPPosition, projection);
//		//�O�̃t���[���̃r���[�v���W�F�N�V�����ϊ�
//		beforeVPPosition = mul(float4(beforePosition, 1), beforeView);
//		beforeVPPosition = mul(beforeVPPosition, beforeProjection);
//		//���_�̈ړ��x�N�g�����o��
//		velocity = VPPosition.xyz - beforeVPPosition.xyz;
//		//�@����velocity�̓��ς���O�̃t���[���̕����Ɉ������΂������߂�
//		judge = step(0, dot(normalize(velocity), normal));
//		gout.position = lerp(beforeVPPosition, VPPosition, judge);
//		//���x�x�N�g���̌v�Z
//		gout.velocity.xy = (VPPosition.xy / VPPosition.w - beforeVPPosition.xy / beforeVPPosition.w) * 0.5f;
//		gout.velocity.y *= -1.0f;
//		gout.velocity.zw = gout.position.zw;
//		output.Append(gout);
//		output.RestartStrip();
//
//		/*************Line**************/
//		//���t���[���ƑO�̃t���[����position�̎擾
//		position = input[0].position.xyz + mul(lineBox[2 + i * 4] * input[0].scale, rotateMatrix);
//		beforePosition = input[0].position.xyz - input[0].velocity * elapsdTime + mul(lineBox[2 + i * 4] * input[0].scale, beforeRotateMatrix);
//		//���t���[���̃r���[�v���W�F�N�V�����ϊ�
//		VPPosition = mul(float4(position, 1), view);
//		VPPosition = mul(VPPosition, projection);
//		//�O�̃t���[���̃r���[�v���W�F�N�V�����ϊ�
//		beforeVPPosition = mul(float4(beforePosition, 1), beforeView);
//		beforeVPPosition = mul(beforeVPPosition, beforeProjection);
//		//���_�̈ړ��x�N�g�����o��
//		velocity = VPPosition.xyz - beforeVPPosition.xyz;
//		//�@����velocity�̓��ς���O�̃t���[���̕����Ɉ������΂������߂�
//		judge = step(0, dot(normalize(velocity), normal));
//		gout.position = lerp(beforeVPPosition, VPPosition, judge);
//		//���x�x�N�g���̌v�Z
//		gout.velocity.xy = (VPPosition.xy / VPPosition.w - beforeVPPosition.xy / beforeVPPosition.w) * 0.5f;
//		gout.velocity.y *= -1.0f;
//		gout.velocity.zw = gout.position.zw;
//		output.Append(gout);
//		//���t���[���ƑO�̃t���[����position�̎擾
//		position = input[0].position.xyz + mul(lineBox[3 + i * 4] * input[0].scale, rotateMatrix);
//		beforePosition = input[0].position.xyz - input[0].velocity * elapsdTime + mul(lineBox[3 + i * 4] * input[0].scale, beforeRotateMatrix);
//		//���t���[���̃r���[�v���W�F�N�V�����ϊ�
//		VPPosition = mul(float4(position, 1), view);
//		VPPosition = mul(VPPosition, projection);
//		//�O�̃t���[���̃r���[�v���W�F�N�V�����ϊ�
//		beforeVPPosition = mul(float4(beforePosition, 1), beforeView);
//		beforeVPPosition = mul(beforeVPPosition, beforeProjection);
//		//���_�̈ړ��x�N�g�����o��
//		velocity = VPPosition.xyz - beforeVPPosition.xyz;
//		//�@����velocity�̓��ς���O�̃t���[���̕����Ɉ������΂������߂�
//		judge = step(0, dot(normalize(velocity), normal));
//		gout.position = lerp(beforeVPPosition, VPPosition, judge);
//		//���x�x�N�g���̌v�Z
//		gout.velocity.xy = (VPPosition.xy / VPPosition.w - beforeVPPosition.xy / beforeVPPosition.w) * 0.5f;
//		gout.velocity.y *= -1.0f;
//		gout.velocity.zw = gout.position.zw;
//		output.Append(gout);
//		output.RestartStrip();
//
//		//���t���[���̖@�����Z�o
//		normal = normalize(cross(lineBox[(0 + i * 4) % 8] - lineBox[(3 + i * 4) % 8], lineBox[(6 + i * 4) % 8] - lineBox[(3 + i * 4) % 8]));
//		/*************Line**************/
//		//���t���[���ƑO�̃t���[����position�̎擾
//		position = input[0].position.xyz + mul(lineBox[(0 + i * 4) % 8] * input[0].scale, rotateMatrix);
//		beforePosition = input[0].position.xyz - input[0].velocity * elapsdTime + mul(lineBox[(0 + i * 4) % 8] * input[0].scale, beforeRotateMatrix);
//		//���t���[���̃r���[�v���W�F�N�V�����ϊ�
//		VPPosition = mul(float4(position, 1), view);
//		VPPosition = mul(VPPosition, projection);
//		//�O�̃t���[���̃r���[�v���W�F�N�V�����ϊ�
//		beforeVPPosition = mul(float4(beforePosition, 1), beforeView);
//		beforeVPPosition = mul(beforeVPPosition, beforeProjection);
//		//���_�̈ړ��x�N�g�����o��
//		velocity = VPPosition.xyz - beforeVPPosition.xyz;
//		//�@����velocity�̓��ς���O�̃t���[���̕����Ɉ������΂������߂�
//		judge = step(0, dot(normalize(velocity), normal));
//		gout.position = lerp(beforeVPPosition, VPPosition, judge);
//		//���x�x�N�g���̌v�Z
//		gout.velocity.xy = (VPPosition.xy / VPPosition.w - beforeVPPosition.xy / beforeVPPosition.w) * 0.5f;
//		gout.velocity.y *= -1.0f;
//		gout.velocity.zw = gout.position.zw;
//		output.Append(gout);
//		//���t���[���ƑO�̃t���[����position�̎擾
//		position = input[0].position.xyz + mul(lineBox[(3 + i * 4) % 8] * input[0].scale, rotateMatrix);
//		beforePosition = input[0].position.xyz - input[0].velocity * elapsdTime + mul(lineBox[(3 + i * 4) % 8] * input[0].scale, beforeRotateMatrix);
//		//���t���[���̃r���[�v���W�F�N�V�����ϊ�
//		VPPosition = mul(float4(position, 1), view);
//		VPPosition = mul(VPPosition, projection);
//		//�O�̃t���[���̃r���[�v���W�F�N�V�����ϊ�
//		beforeVPPosition = mul(float4(beforePosition, 1), beforeView);
//		beforeVPPosition = mul(beforeVPPosition, beforeProjection);
//		//���_�̈ړ��x�N�g�����o��
//		velocity = VPPosition.xyz - beforeVPPosition.xyz;
//		//�@����velocity�̓��ς���O�̃t���[���̕����Ɉ������΂������߂�
//		judge = step(0, dot(normalize(velocity), normal));
//		gout.position = lerp(beforeVPPosition, VPPosition, judge);
//		//���x�x�N�g���̌v�Z
//		gout.velocity.xy = (VPPosition.xy / VPPosition.w - beforeVPPosition.xy / beforeVPPosition.w) * 0.5f;
//		gout.velocity.y *= -1.0f;
//		gout.velocity.zw = gout.position.zw;
//		output.Append(gout);
//		output.RestartStrip();
//		/*************Line**************/
//        //���t���[���ƑO�̃t���[����position�̎擾
//		position = input[0].position.xyz + mul(lineBox[(6 + i * 4) % 8] * input[0].scale, rotateMatrix);
//		beforePosition = input[0].position.xyz - input[0].velocity * elapsdTime + mul(lineBox[(6 + i * 4) % 8] * input[0].scale, beforeRotateMatrix);
//		//���t���[���̃r���[�v���W�F�N�V�����ϊ�
//		VPPosition = mul(float4(position, 1), view);
//		VPPosition = mul(VPPosition, projection);
//		//�O�̃t���[���̃r���[�v���W�F�N�V�����ϊ�
//		beforeVPPosition = mul(float4(beforePosition, 1), beforeView);
//		beforeVPPosition = mul(beforeVPPosition, beforeProjection);
//		//���_�̈ړ��x�N�g�����o��
//		velocity = VPPosition.xyz - beforeVPPosition.xyz;
//		//�@����velocity�̓��ς���O�̃t���[���̕����Ɉ������΂������߂�
//		judge = step(0, dot(normalize(velocity), normal));
//		gout.position = lerp(beforeVPPosition, VPPosition, judge);
//		//���x�x�N�g���̌v�Z
//		gout.velocity.xy = (VPPosition.xy / VPPosition.w - beforeVPPosition.xy / beforeVPPosition.w) * 0.5f;
//		gout.velocity.y *= -1.0f;
//		gout.velocity.zw = gout.position.zw;
//		output.Append(gout);
//		//���t���[���ƑO�̃t���[����position�̎擾
//		position = input[0].position.xyz + mul(lineBox[(3 + i * 4) % 8] * input[0].scale, rotateMatrix);
//		beforePosition = input[0].position.xyz - input[0].velocity * elapsdTime + mul(lineBox[(3 + i * 4) % 8] * input[0].scale, beforeRotateMatrix);
//		//���t���[���̃r���[�v���W�F�N�V�����ϊ�
//		VPPosition = mul(float4(position, 1), view);
//		VPPosition = mul(VPPosition, projection);
//		//�O�̃t���[���̃r���[�v���W�F�N�V�����ϊ�
//		beforeVPPosition = mul(float4(beforePosition, 1), beforeView);
//		beforeVPPosition = mul(beforeVPPosition, beforeProjection);
//		//���_�̈ړ��x�N�g�����o��
//		velocity = VPPosition.xyz - beforeVPPosition.xyz;
//		//�@����velocity�̓��ς���O�̃t���[���̕����Ɉ������΂������߂�
//		judge = step(0, dot(normalize(velocity), normal));
//		gout.position = lerp(beforeVPPosition, VPPosition, judge);
//		//���x�x�N�g���̌v�Z
//		gout.velocity.xy = (VPPosition.xy / VPPosition.w - beforeVPPosition.xy / beforeVPPosition.w) * 0.5f;
//		gout.velocity.y *= -1.0f;
//		gout.velocity.zw = gout.position.zw;
//		output.Append(gout);
//		output.RestartStrip();
//
//		//���t���[���̖@�����Z�o
//		normal = normalize(cross(lineBox[(0 + i * 4) % 8] - lineBox[(5 + i * 4) % 8], lineBox[(6 + i * 4) % 8] - lineBox[(5 + i * 4) % 8]));
//		/*************Line**************/
//        //���t���[���ƑO�̃t���[����position�̎擾
//		position = input[0].position.xyz + mul(lineBox[(0 + i * 4) % 8] * input[0].scale, rotateMatrix);
//		beforePosition = input[0].position.xyz - input[0].velocity * elapsdTime + mul(lineBox[(0 + i * 4) % 8] * input[0].scale, beforeRotateMatrix);
//		//���t���[���̃r���[�v���W�F�N�V�����ϊ�
//		VPPosition = mul(float4(position, 1), view);
//		VPPosition = mul(VPPosition, projection);
//		//�O�̃t���[���̃r���[�v���W�F�N�V�����ϊ�
//		beforeVPPosition = mul(float4(beforePosition, 1), beforeView);
//		beforeVPPosition = mul(beforeVPPosition, beforeProjection);
//		//���_�̈ړ��x�N�g�����o��
//		velocity = VPPosition.xyz - beforeVPPosition.xyz;
//		//�@����velocity�̓��ς���O�̃t���[���̕����Ɉ������΂������߂�
//		judge = step(0, dot(normalize(velocity), normal));
//		gout.position = lerp(beforeVPPosition, VPPosition, judge);
//		//���x�x�N�g���̌v�Z
//		gout.velocity.xy = (VPPosition.xy / VPPosition.w - beforeVPPosition.xy / beforeVPPosition.w) * 0.5f;
//		gout.velocity.y *= -1.0f;
//		gout.velocity.zw = gout.position.zw;
//		output.Append(gout);
//		//���t���[���ƑO�̃t���[����position�̎擾
//		position = input[0].position.xyz + mul(lineBox[(5 + i * 4) % 8] * input[0].scale, rotateMatrix);
//		beforePosition = input[0].position.xyz - input[0].velocity * elapsdTime + mul(lineBox[(5 + i * 4) % 8] * input[0].scale, beforeRotateMatrix);
//		//���t���[���̃r���[�v���W�F�N�V�����ϊ�
//		VPPosition = mul(float4(position, 1), view);
//		VPPosition = mul(VPPosition, projection);
//		//�O�̃t���[���̃r���[�v���W�F�N�V�����ϊ�
//		beforeVPPosition = mul(float4(beforePosition, 1), beforeView);
//		beforeVPPosition = mul(beforeVPPosition, beforeProjection);
//		//���_�̈ړ��x�N�g�����o��
//		velocity = VPPosition.xyz - beforeVPPosition.xyz;
//		//�@����velocity�̓��ς���O�̃t���[���̕����Ɉ������΂������߂�
//		judge = step(0, dot(normalize(velocity), normal));
//		gout.position = lerp(beforeVPPosition, VPPosition, judge);
//		//���x�x�N�g���̌v�Z
//		gout.velocity.xy = (VPPosition.xy / VPPosition.w - beforeVPPosition.xy / beforeVPPosition.w) * 0.5f;
//		gout.velocity.y *= -1.0f;
//		gout.velocity.zw = gout.position.zw;
//		output.Append(gout);
//		output.RestartStrip();
//		/*************Line**************/
//		//���t���[���ƑO�̃t���[����position�̎擾
//		position = input[0].position.xyz + mul(lineBox[(6 + i * 4) % 8] * input[0].scale, rotateMatrix);
//		beforePosition = input[0].position.xyz - input[0].velocity * elapsdTime + mul(lineBox[(6 + i * 4) % 8] * input[0].scale, beforeRotateMatrix);
//		//���t���[���̃r���[�v���W�F�N�V�����ϊ�
//		VPPosition = mul(float4(position, 1), view);
//		VPPosition = mul(VPPosition, projection);
//		//�O�̃t���[���̃r���[�v���W�F�N�V�����ϊ�
//		beforeVPPosition = mul(float4(beforePosition, 1), beforeView);
//		beforeVPPosition = mul(beforeVPPosition, beforeProjection);
//		//���_�̈ړ��x�N�g�����o��
//		velocity = VPPosition.xyz - beforeVPPosition.xyz;
//		//�@����velocity�̓��ς���O�̃t���[���̕����Ɉ������΂������߂�
//		judge = step(0, dot(normalize(velocity), normal));
//		gout.position = lerp(beforeVPPosition, VPPosition, judge);
//		//���x�x�N�g���̌v�Z
//		gout.velocity.xy = (VPPosition.xy / VPPosition.w - beforeVPPosition.xy / beforeVPPosition.w) * 0.5f;
//		gout.velocity.y *= -1.0f;
//		gout.velocity.zw = gout.position.zw;
//		output.Append(gout);
//		//���t���[���ƑO�̃t���[����position�̎擾
//		position = input[0].position.xyz + mul(lineBox[(5 + i * 4) % 8] * input[0].scale, rotateMatrix);
//		beforePosition = input[0].position.xyz - input[0].velocity * elapsdTime + mul(lineBox[(5 + i * 4) % 8] * input[0].scale, beforeRotateMatrix);
//		//���t���[���̃r���[�v���W�F�N�V�����ϊ�
//		VPPosition = mul(float4(position, 1), view);
//		VPPosition = mul(VPPosition, projection);
//		//�O�̃t���[���̃r���[�v���W�F�N�V�����ϊ�
//		beforeVPPosition = mul(float4(beforePosition, 1), beforeView);
//		beforeVPPosition = mul(beforeVPPosition, beforeProjection);
//		//���_�̈ړ��x�N�g�����o��
//		velocity = VPPosition.xyz - beforeVPPosition.xyz;
//		//�@����velocity�̓��ς���O�̃t���[���̕����Ɉ������΂������߂�
//		judge = step(0, dot(normalize(velocity), normal));
//		gout.position = lerp(beforeVPPosition, VPPosition, judge);
//		//���x�x�N�g���̌v�Z
//		gout.velocity.xy = (VPPosition.xy / VPPosition.w - beforeVPPosition.xy / beforeVPPosition.w) * 0.5f;
//		gout.velocity.y *= -1.0f;
//		gout.velocity.zw = gout.position.zw;
//		output.Append(gout);
//		output.RestartStrip();
//
//	}
//	
//}
void main(
	point VS_OUT input[1],
	inout TriangleStream<GS_OUT > output
)
{

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

	float3 position = (float3)0, beforePosition = (float3)0;
	float3 normal = (float3)0, velocity = (float3)0;
	float4 beforeVPPosition = (float4)0, VPPosition = (float4)0;
	float3x3 rotateMatrix = mul(MakeRotation(float3(1, 0, 0), input[0].angle.x), mul(MakeRotation(float3(0, 1, 0), input[0].angle.y), MakeRotation(float3(0, 0, 1), input[0].angle.z)));
	float3 beforeAngle = input[0].angle - angleMovement * elapsdTime;
	float3x3 beforeRotateMatrix = mul(MakeRotation(float3(1, 0, 0), beforeAngle.x), mul(MakeRotation(float3(0, 1, 0), beforeAngle.y), MakeRotation(float3(0, 0, 1), beforeAngle.z)));
	for (int i = 0; i < 12; i++)
	{
		//�@��
		normal = mul(cross(triangleBox[index[i * 3]] - triangleBox[index[i * 3 + 1]], triangleBox[index[i * 3 + 2]] - triangleBox[index[i * 3 + 1]]), (float3x3)mul(view, projection));
		GS_OUT gout;
		/************************���_************************/
		//���̃t���[���̍��W
		position = input[0].position.xyz + mul(triangleBox[index[i * 3]] * input[0].scale, rotateMatrix);
		VPPosition = mul(float4(position, 1), view);
		VPPosition = mul(VPPosition, projection);
		//�O�̃t���[���̍��W
		beforePosition = input[0].position.xyz - input[0].velocity * elapsdTime + mul(triangleBox[index[i * 3]] * input[0].scale, beforeRotateMatrix);
		beforeVPPosition = mul(float4(beforePosition, 1), beforeView);
		beforeVPPosition = mul(beforeVPPosition, beforeProjection);
		//�ړ�����
		velocity = VPPosition.xyz - beforePosition.xyz;

		float d = dot(normalize(velocity), normalize(normal));
		if (d < 0.0f)gout.position = beforeVPPosition;
		else gout.position = VPPosition;
		gout.position = beforeVPPosition;
		//���x
		gout.velocity.xy = ((VPPosition.xy / VPPosition.z) - (beforeVPPosition.xy / beforeVPPosition.z)) * 0.5f;
		gout.velocity.y *= -1.0f;
		gout.velocity.z = gout.position.z;
		gout.velocity.w = gout.position.w;

		output.Append(gout);
		/************************���_************************/

		//���̃t���[���̍��W
		position = input[0].position.xyz + mul(triangleBox[index[i * 3 + 1]] * input[0].scale, rotateMatrix);
		VPPosition = mul(float4(position, 1), view);
		VPPosition = mul(VPPosition, projection);
		//�O�̃t���[���̍��W
		beforePosition = input[0].position.xyz - input[0].velocity * elapsdTime + mul(triangleBox[index[i * 3 + 1]] * input[0].scale, beforeRotateMatrix);
		beforeVPPosition = mul(float4(beforePosition, 1), beforeView);
		beforeVPPosition = mul(beforeVPPosition, beforeProjection);
		//�ړ�����
		velocity = VPPosition.xyz - beforePosition.xyz;

		d = dot(normalize(velocity), normalize(normal));
		if (d < 0.0f)gout.position = beforeVPPosition;
		else gout.position = VPPosition;
		//���x
		gout.velocity.xy = ((VPPosition.xy / VPPosition.z) - (beforeVPPosition.xy / beforeVPPosition.z)) * 0.5f;
		gout.velocity.y *= -1.0f;
		gout.velocity.z = gout.position.z;
		gout.velocity.w = gout.position.w;
		output.Append(gout);
		/************************���_************************/

		//���̃t���[���̍��W
		position = input[0].position.xyz + mul(triangleBox[index[i * 3 + 2]] * input[0].scale, rotateMatrix);
		VPPosition = mul(float4(position, 1), view);
		VPPosition = mul(VPPosition, projection);
		//�O�̃t���[���̍��W
		beforePosition = input[0].position.xyz - input[0].velocity * elapsdTime + mul(triangleBox[index[i * 3 + 2]] * input[0].scale, beforeRotateMatrix);
		beforeVPPosition = mul(float4(beforePosition, 1), beforeView);
		beforeVPPosition = mul(beforeVPPosition, beforeProjection);
		//�ړ�����
		velocity = VPPosition.xyz - beforePosition.xyz;

		d = dot(normalize(velocity), normalize(normal));
		if (d < 0.0f)gout.position = beforeVPPosition;
		else gout.position = VPPosition;
		//���x
		gout.velocity.xy = ((VPPosition.xy / VPPosition.z) - (beforeVPPosition.xy / beforeVPPosition.z)) * 0.5f;
		gout.velocity.y *= -1.0f;
		gout.velocity.z = gout.position.z;
		gout.velocity.w = gout.position.w;
		output.Append(gout);
		output.RestartStrip();
	}

}