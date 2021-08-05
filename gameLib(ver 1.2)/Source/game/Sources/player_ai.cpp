#include "player_ai.h"
//#include"camera_manager.h"
//#include"Judgment.h"
//#include"hit_area_render.h"
#include"file_function.h"

#ifdef USE_IMGUI
#include<imgui.h>
#endif
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�������֐�(�R���X�g���N�^)
/*****************************************************/
PlayerAI::PlayerAI(PlayerCharacter* character) : mGravity(0)
{
	////player�I�u�W�F�N�g�̐���
	//std::unique_ptr<ModelData>data = std::make_unique<ModelData>(fileName);
	//std::shared_ptr<ModelResource>resouce = std::make_shared<ModelResource>(device, std::move(data));
	//mCharacter = std::make_unique<PlayerCharacter>(resouce);
	//�������W�̐ݒ�
	character->SetPosition(VECTOR3F(0, 10, 0));
	character->SetScale(VECTOR3F(gameObjScale, gameObjScale, gameObjScale));
	character->CalculateBoonTransform(0);
	//�p�����[�^�[�̐ݒ�
	memset(&mParameter, 0, sizeof(mParameter));
	FileFunction::Load(mParameter, "Data/file/playerParrameter.bin", "rb");
	character->SetMinSpeed(mParameter.minSpeed);
	character->SetMaxSpeed(mParameter.maxSpeed);
}

/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�G�f�B�^�֐�
/*****************************************************/
void PlayerAI::Editor(PlayerCharacter* character)
{
#ifdef USE_IMGUI
	ImGui::Begin("player");
	//�����x
	float* accel[3] = { &mParameter.accel.x,&mParameter.accel.y ,&mParameter.accel.z };
	ImGui::InputFloat("accel", &mParameter.accel.z, 10);
	//�W�����v��
	float* j[3] = { &mParameter.jump.x,&mParameter.jump.y,&mParameter.jump.z };
	ImGui::InputFloat("jump", &mParameter.jump.y, 10);
	//�W�����v��Ŕ�ԗ�
	float* r[3] = { &mParameter.ranp.x,&mParameter.ranp.y,&mParameter.ranp.z };
	ImGui::InputFloat("ranp", &mParameter.ranp.y, 10);
	//�ő呬�x
	if (ImGui::InputFloat("maxSpeed", &mParameter.maxSpeed, 10))
	{
		character->SetMaxSpeed(mParameter.maxSpeed);
	}
	//�ŏ����x
	if (ImGui::InputFloat("minSpeed", &mParameter.minSpeed, 10))
	{
		character->SetMinSpeed(mParameter.minSpeed);
	}
	//�d�͒l
	ImGui::InputFloat("gravity", &mParameter.gravity, 10);
	//�Z�[�u
	if (ImGui::Button("save"))
	{
		FileFunction::Save(mParameter, "Data/file/playerParrameter.bin", "wb");

	}
	//���[�h
	if (ImGui::Button("load"))
	{
		FileFunction::Load(mParameter, "Data/file/playerParrameter.bin", "rb");

	}
	//�p�����[�^�[�̕\��
	ImGui::Text("position:%f,%f,%f", character->GetPosition().x, character->GetPosition().y, character->GetPosition().z);
	ImGui::Text("velocity:%f,%f,%f", character->GetVelocity().x, character->GetVelocity().y, character->GetVelocity().z);
	ImGui::Text("state:%d", character->GetMoveState());
	ImGui::Text("changeFlg:%d", character->GetChangState());
	ImGui::End();
#endif
}

/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�X�V�֐�
/*****************************************************/
/********************player�̍X�V******************/

void PlayerAI::Update(float elapsd_time, PlayerCharacter*character)
{
	//���x�A�����x�̎擾
	VECTOR3F accel = character->GetAccel();
	VECTOR3F velocity = character->GetVelocity();
	//��Ԃ̎擾
	auto state = character->GetMoveState();
	//�����x��ݒ�
	accel.z = mParameter.accel.z;
	//�d�͒l���X�V
	mGravity += mParameter.gravity * elapsd_time;
	//�����x�ɏd�͒l�𑫂�
	accel.y += mGravity * 60 * elapsd_time;
	//�󒆂ɂ��Ȃ����ǂ����𔻒肷��
	if (character->GetGroundFlag())
	{
		mGravity = mParameter.gravity;
		velocity.y = 0;
		accel.y = 0;
	}
	//��Ԃɍ��킹�đ��x��ω�������
	if (state == PlayerCharacter::MOVESTATE::JUMP)
	{
		velocity.y = mParameter.jump.y;
		state = PlayerCharacter::MOVESTATE::LANDING;
	}
	if (state == PlayerCharacter::MOVESTATE::RAMP)
	{
		velocity.y = mParameter.ranp.y;
		state = PlayerCharacter::MOVESTATE::LANDING;
	}
	//�S�[���������̃L�����N�^�[�̓���
	if (character->GetGorlFlag())
	{
		VECTOR3F angle = character->GetAngle();
		if (angle.y < DirectX::XMConvertToRadians(135))
		{
			angle.y += 3.14f * elapsd_time * 1.05f;
			character->SetAngle(angle);
		}
		accel.z = -velocity.z * 3.f;
		accel.y = 0;
		velocity.y = 0;
	}
	//�X�V�����f�[�^�̃Z�b�g
	character->SetAccel(accel);
	character->SetVelocity(velocity);
	character->SetMoveState(state);

}

void PlayerAI::Reset()
{
	mGravity = mParameter.gravity;

}
