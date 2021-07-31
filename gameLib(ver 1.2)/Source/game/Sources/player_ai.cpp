#include "player_ai.h"
#include"camera_manager.h"
#include"Judgment.h"
#include"hit_area_render.h"
#include"file_function.h"

#ifdef USE_IMGUI
#include<imgui.h>
#endif
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�������֐�(�R���X�g���N�^)
/*****************************************************/
PlayerAI::PlayerAI(ID3D11Device* device, const char* fileName) :mPlayFlag(false), mGravity(0)
{
	//player�I�u�W�F�N�g�̐���
	std::unique_ptr<ModelData>data = std::make_unique<ModelData>(fileName);
	std::shared_ptr<ModelResource>resouce = std::make_shared<ModelResource>(device, std::move(data));
	mCharacter = std::make_unique<PlayerCharacter>(resouce);
	//�������W�̐ݒ�
	mCharacter->SetPosition(VECTOR3F(0, 10, 0));
	mCharacter->SetScale(VECTOR3F(gameObjScale, gameObjScale, gameObjScale));
	mCharacter->CalculateBoonTransform(0);
	//�p�����[�^�[�̐ݒ�
	memset(&mParameter, 0, sizeof(mParameter));
	FileFunction::Load(mParameter, "Data/file/playerParrameter.bin", "rb");
	mCharacter->SetMinSpeed(mParameter.minSpeed);
	mCharacter->SetMaxSpeed(mParameter.maxSpeed);
}

/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�G�f�B�^�֐�
/*****************************************************/
void PlayerAI::Editor()
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
		mCharacter->SetMaxSpeed(mParameter.maxSpeed);
	}
	//�ŏ����x
	if (ImGui::InputFloat("minSpeed", &mParameter.minSpeed, 10))
	{
		mCharacter->SetMinSpeed(mParameter.minSpeed);
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
	//player���W�Ȃǂ̃��Z�b�g
	if (ImGui::Button("replay"))
	{
		mCharacter->SetPosition(VECTOR3F(0, 10, 0));
		mCharacter->SetBeforePosition(VECTOR3F(0, 10, 0));
		mCharacter->SetVelocity(VECTOR3F(0, 0, 0));
		mCharacter->SetAngle(VECTOR3F(0, 0, 0));
		mCharacter->CalculateBoonTransform(0);
		mCharacter->SetMoveState(PlayerCharacter::MOVESTATE::LANDING);

	}
	//�p�����[�^�[�̕\��
	ImGui::Text("position:%f,%f,%f", mCharacter->GetPosition().x, mCharacter->GetPosition().y, mCharacter->GetPosition().z);
	ImGui::Text("velocity:%f,%f,%f", mCharacter->GetVelocity().x, mCharacter->GetVelocity().y, mCharacter->GetVelocity().z);
	ImGui::Text("state:%d", mCharacter->GetMoveState());
	ImGui::Text("changeFlg:%d", mCharacter->GetChangState());
	ImGui::End();
#endif
}

/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�X�V�֐�
/*****************************************************/
/********************player�̍X�V******************/

void PlayerAI::Update(float elapsd_time, StageManager* manager, StageOperation* operation)
{
	if (mPlayFlag)
	{	//�v���C��

		//���ȏ㉺�ɗ�������X�^�[�g���_�ɖ߂�
		if (mCharacter->GetPosition().y < -1000)
		{
			if (mCharacter->GetExist())
			{
				mCharacter->SetExist(false);
				operation->Reset(manager);

			}
			return;
		}
		//������Ƃ�
		Move(elapsd_time);
	}
	else
	{//�v���C���ĂȂ���
		elapsd_time = 0;
		mCharacter->SetGorlFlag(false);
		mCharacter->SetGroundFlag(false);
		mGravity = mParameter.gravity;
	}
	//�X�e�[�W�Ƃ̓����蔻��
	Judgment::Judge(mCharacter.get(), manager);
	//�J������player�̈ʒu��ݒ�
	pCameraManager->GetCameraOperation()->GetPlayCamera()->SetPlayerPosition(mCharacter->GetPosition());
	//�A�j���[�V�����̍X�V
	mCharacter->AnimUpdate(elapsd_time);
}
/********************player�𓮂���******************/
void PlayerAI::Move(float elapsd_time)
{
	//���x�A�����x�̎擾
	VECTOR3F accel = mCharacter->GetAccel();
	VECTOR3F velocity = mCharacter->GetVelocity();
	//��Ԃ̎擾
	auto state = mCharacter->GetMoveState();
	//�����x��ݒ�
	accel.z = mParameter.accel.z;
	//�d�͒l���X�V
	mGravity += mParameter.gravity * elapsd_time;
	//�����x�ɏd�͒l�𑫂�
	accel.y += mGravity * 60 * elapsd_time;
	//�󒆂ɂ��Ȃ����ǂ����𔻒肷��
	if (mCharacter->GetGroundFlag())
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
	if (mCharacter->GetGorlFlag())
	{
		VECTOR3F angle = mCharacter->GetAngle();
		if (angle.y < DirectX::XMConvertToRadians(135))
		{
			angle.y += 3.14f * elapsd_time * 1.05f;
			mCharacter->SetAngle(angle);
		}
		accel.z = -velocity.z * 3.f;
		accel.y = 0;
		velocity.y = 0;
	}
	//�X�V�����f�[�^�̃Z�b�g
	mCharacter->SetAccel(accel);
	mCharacter->SetVelocity(velocity);
	mCharacter->SetMoveState(state);
	//���W�Ȃǂ̍X�V
	mCharacter->Move(elapsd_time);

}

void PlayerAI::Respond()
{
	auto camera = pCameraManager->GetCameraOperation()->GetPlayCamera();

	mCharacter->SetPosition(VECTOR3F(0, 10, 0));
	mCharacter->SetBeforePosition(VECTOR3F(0, 10, 0));
	mCharacter->SetVelocity(VECTOR3F(0, 0, 0));
	mCharacter->SetAngle(VECTOR3F(0, 0, 0));
	mCharacter->CalculateBoonTransform(0);
	mCharacter->SetAccel(VECTOR3F(0, 0, 0));
	mGravity = mParameter.gravity;
	camera->Reset();
	mCharacter->SetExist(true);
}
