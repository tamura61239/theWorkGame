#include "player_ai.h"
#include"camera_manager.h"
#include"Judgment.h"

#ifdef USE_IMGUI
#include<imgui.h>
#endif
PlayerAI::PlayerAI(ID3D11Device* device, const char* fileName)
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
	if (fopen_s(&fp, "Data/file/playerCameraParameter.bin", "rb") == 0)
	{
		fread(&mCameraParameter, sizeof(CameraParameter), 1, fp);
		fclose(fp);
	}
	else
	{
		mCameraParameter.angle = 3.14f * 0.75f;
		mCameraParameter.length = 60;
		mCameraParameter.y = 25;
	}
}

void PlayerAI::Save()
{
	FILE* fp;
	fopen_s(&fp, "Data/file/playerParrameter.bin", "wb");
	fwrite(&mParameter, sizeof(PlayerParameter), 1, fp);
	fclose(fp);

	fopen_s(&fp, "Data/file/playerCameraParameter.bin", "wb");
	fwrite(&mCameraParameter, sizeof(CameraParameter), 1, fp);
	fclose(fp);

}
static bool play = false;
void PlayerAI::ImGuiUpdate()
{
#ifdef USE_IMGUI
	ImGui::Begin("player");
	float* accel[3] = { &mParameter.accel.x,&mParameter.accel.y ,&mParameter.accel.z };
	ImGui::SliderFloat3("accel", *accel, 0, 500);
	float* j[3] = { &mParameter.jump.x,&mParameter.jump.y,&mParameter.jump.z };
	ImGui::SliderFloat3("jump", *j, 0, 1700);
	float* r[3] = { &mParameter.ranp.x,&mParameter.ranp.y,&mParameter.ranp.z };
	ImGui::SliderFloat3("ranp", *r, 0, 3500);
	if (ImGui::SliderFloat("maxSpeed", &mParameter.maxSpeed, mParameter.minSpeed, 1500))
	{
		mCharacter->SetMaxSpeed(mParameter.maxSpeed);
	}
	if (ImGui::SliderFloat("minSpeed", &mParameter.minSpeed, 0, mParameter.maxSpeed))
	{
		mCharacter->SetMinSpeed(mParameter.minSpeed);
	}
	ImGui::InputFloat("gravity", &mParameter.gravity);
	ImGui::Text("camera data");
	ImGui::SliderFloat("camera angle", &mCameraParameter.angle, -3.14f, 3.14f);
	ImGui::InputFloat("camera length", &mCameraParameter.length, 10);
	ImGui::InputFloat("camera y", &mCameraParameter.y, 5);
	if (ImGui::Button("save"))
	{
		Save();
	}
	if (ImGui::Button("replay"))
	{
		mCharacter->SetPosition(VECTOR3F(0, 10, 0));
		mCharacter->SetBeforePosition(VECTOR3F(0, 10, 0));
		mCharacter->SetVelocity(VECTOR3F(0, 0, 0));
		mCharacter->CalculateBoonTransform(0);
		mCharacter->SetMoveState(PlayerCharacter::MOVESTATE::LANDING);
		pCamera.GetCamera()->SetEye(mCharacter->GetPosition() + VECTOR3F(sinf(mCameraParameter.angle) * mCameraParameter.length, mCameraParameter.y, cosf(mCameraParameter.angle) * mCameraParameter.length) * gameObjScale);
		pCamera.GetCamera()->SetFocus(mCharacter->GetPosition());
		pCamera.GetCamera()->SetUp(VECTOR3F(0, 1, 0));

	}
	ImGui::Checkbox("play", &play);
	ImGui::Text("position:%f,%f,%f", mCharacter->GetPosition().x, mCharacter->GetPosition().y, mCharacter->GetPosition().z);
	ImGui::Text("velocity:%f,%f,%f", mCharacter->GetVelocity().x, mCharacter->GetVelocity().y, mCharacter->GetVelocity().z);
	ImGui::Text("state:%d", mCharacter->GetMoveState());
	ImGui::End();
#endif
}

void PlayerAI::Update(float elapsd_time, StageManager* manager)
{
	ImGuiUpdate();
	if (!play)
	{
		if (pCamera.GetCameraOperation()->GetCameraType() == CameraOperation::CAMERA_TYPE::DEBUG)return;
		pCamera.GetCamera()->SetEye(mCharacter->GetPosition() + VECTOR3F(sinf(mCameraParameter.angle) * mCameraParameter.length, mCameraParameter.y, cosf(mCameraParameter.angle) * mCameraParameter.length) * gameObjScale);
		pCamera.GetCamera()->SetFocus(mCharacter->GetPosition());
		pCamera.GetCamera()->SetUp(VECTOR3F(0, 1, 0));
		return;
	}
	pCamera.GetCameraOperation()->SetCameraType(CameraOperation::CAMERA_TYPE::NORMAL);
	if (mCharacter->GetPosition().y < -1000)
	{
		play = false;
		mCharacter->SetPosition(VECTOR3F(0, 10, 0));
		mCharacter->SetBeforePosition(VECTOR3F(0, 10, 0));
		mCharacter->SetVelocity(VECTOR3F(0, 0, 0));
		mCharacter->CalculateBoonTransform(0);
		pCamera.GetCamera()->SetEye(mCharacter->GetPosition() + VECTOR3F(sinf(mCameraParameter.angle) * mCameraParameter.length, mCameraParameter.y, cosf(mCameraParameter.angle) * mCameraParameter.length) * gameObjScale);
		pCamera.GetCamera()->SetFocus(mCharacter->GetPosition());
		pCamera.GetCamera()->SetUp(VECTOR3F(0, 1, 0));


		return;
	}
	VECTOR3F velocity = mCharacter->GetVelocity();
	VECTOR3F accel = mCharacter->GetAccel();
	static float gravity = 0;
	if (mCharacter->GetGroundFlag())
	{
		accel.y = 0;
		velocity.y = 0;

	}
	accel.y += mParameter.gravity * elapsd_time * 60;
	switch (mCharacter->GetMoveState())
	{
	case PlayerCharacter::MOVESTATE::MOVE:
		accel.x = mParameter.accel.x * 2.f;
		accel.z = mParameter.accel.z * 2.f;
		break;
	case PlayerCharacter::MOVESTATE::JUMP:
		if (mCharacter->GetChangState())
		{
			velocity.y = mParameter.jump.y;
		}
		accel.x = mParameter.accel.x;
		accel.z = mParameter.accel.z;
		mCharacter->SetMoveState(PlayerCharacter::MOVESTATE::LANDING);
		break;
	case PlayerCharacter::MOVESTATE::RAMP:
		if (mCharacter->GetChangState())
		{
			velocity.y = mParameter.ranp.y;
		}
		accel.x = mParameter.accel.x;
		accel.z = mParameter.accel.z;
		mCharacter->SetMoveState(PlayerCharacter::MOVESTATE::LANDING);

		break;
	case PlayerCharacter::MOVESTATE::LANDING:
		accel.x = mParameter.accel.x;
		accel.z = mParameter.accel.z;
		break;
	}
	mCharacter->SetAccel(accel);
	mCharacter->SetVelocity(velocity);
	mCharacter->Move(elapsd_time);
	Judgment::Judge(mCharacter.get(), manager);
	pCamera.GetCamera()->SetEye(mCharacter->GetPosition() + VECTOR3F(sinf(mCameraParameter.angle) * mCameraParameter.length, mCameraParameter.y, cosf(mCameraParameter.angle) * mCameraParameter.length) * gameObjScale);
	pCamera.GetCamera()->SetFocus(mCharacter->GetPosition());
	pCamera.GetCamera()->SetUp(VECTOR3F(0, 1, 0));
	mCharacter->CalculateBoonTransform(elapsd_time);
}
