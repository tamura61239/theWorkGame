#include "player_ai.h"
#include"camera_manager.h"
#include"Judgment.h"
#include"hit_area_render.h"

#ifdef USE_IMGUI
#include<imgui.h>
#endif
PlayerAI::PlayerAI(ID3D11Device* device, const char* fileName):mPlayFlag(false)
{
	std::unique_ptr<ModelData>data = std::make_unique<ModelData>(fileName);
	std::shared_ptr<ModelResource>resouce = std::make_shared<ModelResource>(device, std::move(data));
	mCharacter = std::make_unique<PlayerCharacter>(resouce);
	mCharacter->SetPosition(VECTOR3F(0, 1, 0));
	mCharacter->SetScale(VECTOR3F(gameObjScale, gameObjScale, gameObjScale));
	mCharacter->CalculateBoonTransform(0);
	Load();
}

void PlayerAI::Load()
{
	FILE* fp;
	if (fopen_s(&fp, "Data/file/playerParrameter.bin", "rb") == 0)
	{
		fread(&mParameter, sizeof(PlayerParameter), 1, fp);
		fclose(fp);
		mCharacter->SetMinSpeed(mParameter.minSpeed);
		mCharacter->SetMaxSpeed(mParameter.maxSpeed);
	}
	else
	{
		mParameter.accel = VECTOR3F(0, 0, 0);
		mParameter.maxSpeed = 0;
		mParameter.minSpeed = 0;
		mParameter.jump = VECTOR3F(0, 120, 0);
		mParameter.ranp = VECTOR3F(0, 200, 0);
		mParameter.gravity = -9.8f * 60;
	}
}

void PlayerAI::Save()
{
	FILE* fp;
	fopen_s(&fp, "Data/file/playerParrameter.bin", "wb");
	fwrite(&mParameter, sizeof(PlayerParameter), 1, fp);
	fclose(fp);


}
void PlayerAI::ImGuiUpdate()
{
#ifdef USE_IMGUI
	ImGui::Begin("player");
	float* accel[3] = { &mParameter.accel.x,&mParameter.accel.y ,&mParameter.accel.z };
	//ImGui::SliderFloat3("accel", *accel, 0, 1000);
	ImGui::InputFloat("accel", &mParameter.accel.z,10);
	float* j[3] = { &mParameter.jump.x,&mParameter.jump.y,&mParameter.jump.z };
	//ImGui::SliderFloat3("jump", *j, 0, 1700);
	ImGui::InputFloat("jump", &mParameter.jump.y, 10);
	float* r[3] = { &mParameter.ranp.x,&mParameter.ranp.y,&mParameter.ranp.z };
	//ImGui::SliderFloat3("ranp", *r, 0, 3500);
	ImGui::InputFloat("ranp", &mParameter.ranp.y, 10);
	if (ImGui::InputFloat("maxSpeed", &mParameter.maxSpeed, 10))
	{
		mCharacter->SetMaxSpeed(mParameter.maxSpeed);
	}
	if (ImGui::InputFloat("minSpeed", &mParameter.minSpeed, 10))
	{
		mCharacter->SetMinSpeed(mParameter.minSpeed);
	}
	ImGui::InputFloat("gravity", &mParameter.gravity,10);
	if (ImGui::Button("save"))
	{
		Save();
	}
	if (ImGui::Button("replay"))
	{
		mCharacter->SetPosition(VECTOR3F(0, 10, 0));
		mCharacter->SetBeforePosition(VECTOR3F(0, 10, 0));
		mCharacter->SetVelocity(VECTOR3F(0, 0, 0));
		mCharacter->SetAngle(VECTOR3F(0, 0, 0));
		mCharacter->CalculateBoonTransform(0);
		mCharacter->SetMoveState(PlayerCharacter::MOVESTATE::LANDING);

	}
	ImGui::Checkbox("play", &mPlayFlag);
	ImGui::Text("position:%f,%f,%f", mCharacter->GetPosition().x, mCharacter->GetPosition().y, mCharacter->GetPosition().z);
	ImGui::Text("velocity:%f,%f,%f", mCharacter->GetVelocity().x, mCharacter->GetVelocity().y, mCharacter->GetVelocity().z);
	ImGui::Text("state:%d", mCharacter->GetMoveState());
	ImGui::Text("changeFlg:%d", mCharacter->GetChangState());
	ImGui::End();
#endif
}


void PlayerAI::Update(float elapsd_time, StageManager* manager, StageOperation* operation)
{
	if (!mPlayFlag)
	{
		pCameraManager->GetCameraOperation()->GetPlayCamera()->SetPlayerPosition(mCharacter->GetPosition());
		mCharacter->AnimUpdate(0);
		mCharacter->SetGorlFlag(false);
		HitAreaRender::GetInctance()->SetObjData(mCharacter->GetPosition() + VECTOR3F(0, 3.3f, 0) * mCharacter->GetScale(), VECTOR3F(1.15f, 3.3f, 1.3f) * mCharacter->GetScale());
		mCharacter->SetMoveState(PlayerCharacter::MOVESTATE::LANDING);
		return;
	}
	//pCamera.GetCameraOperation()->SetCameraType(CameraOperation::CAMERA_TYPE::NORMAL);
	if (mCharacter->GetPosition().y < -1000)
	{
		mCharacter->SetPosition(VECTOR3F(0, 10, 0));
		mCharacter->SetBeforePosition(VECTOR3F(0, 10, 0));
		mCharacter->SetVelocity(VECTOR3F(0, 0, 0));
		mCharacter->SetAngle(VECTOR3F(0, 0, 0));
		mCharacter->CalculateBoonTransform(0);
		pCameraManager->GetCameraOperation()->GetPlayCamera()->SetPlayerPosition(mCharacter->GetPosition());
		operation->Reset(manager);
		mCharacter->SetAccel(VECTOR3F(0, 0, 0));
		pCameraManager->Update(elapsd_time);
		return;
	}
	//ゲーム中のキャラクターの動き
	VECTOR3F velocity = mCharacter->GetVelocity();
	VECTOR3F accel = mCharacter->GetAccel();
	if (mCharacter->GetGroundFlag())
	{
		accel.y = 0;
		velocity.y = 0;
	}
	accel.y += mParameter.gravity * elapsd_time * 60;
	switch (mCharacter->GetMoveState())
	{
	case PlayerCharacter::MOVESTATE::MOVE:
		accel.z = mParameter.accel.z * 2.f;
		break;
	case PlayerCharacter::MOVESTATE::JUMP:
		if (mCharacter->GetChangState())
		{
			velocity.y = mParameter.jump.y;
		}
		accel.z = mParameter.accel.z;
		mCharacter->SetMoveState(PlayerCharacter::MOVESTATE::LANDING);
		break;
	case PlayerCharacter::MOVESTATE::RAMP:
		if (mCharacter->GetChangState())
		{
			velocity.y = mParameter.ranp.y;
		}
		accel.z = mParameter.accel.z;
		mCharacter->SetMoveState(PlayerCharacter::MOVESTATE::LANDING);

		break;
	case PlayerCharacter::MOVESTATE::LANDING:
		accel.z = mParameter.accel.z;
		break;
	}
	//ゴールした時のキャラクターの動き
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
		HitAreaRender::GetInctance()->SetObjData(mCharacter->GetPosition() + VECTOR3F(0, 3.3f, 0) * mCharacter->GetScale(), VECTOR3F(1.15f, 3.3f, 1.3f) * mCharacter->GetScale());
	}
	//更新したデータのセット
	mCharacter->SetAccel(accel);
	mCharacter->SetVelocity(velocity);
	//座標などの更新
	mCharacter->Move(elapsd_time);
	//ゴールについてない時の当たり判定
	if (!mCharacter->GetGorlFlag())Judgment::Judge(mCharacter.get(), manager);
	pCameraManager->GetCameraOperation()->GetPlayCamera()->SetPlayerPosition(mCharacter->GetPosition());
	mCharacter->AnimUpdate(elapsd_time);
}
