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
	mCharacter->SetPosition(VECTOR3F(0, 10, 0));
	mCharacter->SetScale(VECTOR3F(10, 10, 10));
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
static bool play = false;
static VECTOR3F jump = VECTOR3F(0, 120, 0);
static VECTOR3F ranp = VECTOR3F(0, 200, 0);
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
		pCamera.GetCamera()->SetEye(mCharacter->GetPosition() + VECTOR3F(600, 250, -600));
		pCamera.GetCamera()->SetFocus(mCharacter->GetPosition());
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
		Judgment::Judge(mCharacter.get(), manager);
		return;
	}
	if (mCharacter->GetPosition().y < -1000)
	{
		play = false;
		mCharacter->SetPosition(VECTOR3F(0, 10, 0));
		mCharacter->SetBeforePosition(VECTOR3F(0, 10, 0));
		mCharacter->SetVelocity(VECTOR3F(0, 0, 0));
		mCharacter->CalculateBoonTransform(0);
		pCamera.GetCamera()->SetEye(mCharacter->GetPosition() + VECTOR3F(600, 250, -600));
		pCamera.GetCamera()->SetFocus(mCharacter->GetPosition());

		return;
	}
	VECTOR3F position = mCharacter->GetPosition();
	mCharacter->SetBeforePosition(position);
	VECTOR3F velocity = mCharacter->GetVelocity();
	VECTOR3F accel = VECTOR3F(0,0,0);
	static float gravity = 0;
	if(mCharacter->GetGroundFlag())gravity = 0;
	switch (mCharacter->GetMoveState())
	{
	case PlayerCharacter::MOVESTATE::MOVE:
		
		accel = mParameter.accel;
		accel += VECTOR3F(0, -9.8f, 0);
		velocity.y = 0;
		break;
	case PlayerCharacter::MOVESTATE::JUMP:
		if (mCharacter->GetChangState())
		{
			velocity.y = 0;
			jump = mParameter.jump;
		}
		accel = mParameter.accel;
		accel += VECTOR3F(0, gravity, 0);
		accel += jump;
		mCharacter->SetMoveState(PlayerCharacter::MOVESTATE::LANDING);
		break;
	case PlayerCharacter::MOVESTATE::RAMP:
		velocity.y = 0;
		accel = mParameter.accel;
		accel += VECTOR3F(0, gravity, 0);
		accel += mParameter.ranp;
		mCharacter->SetMoveState(PlayerCharacter::MOVESTATE::LANDING);
		break;
	case PlayerCharacter::MOVESTATE::LANDING:
		gravity += mParameter.gravity * elapsd_time;
		accel = mParameter.accel;
		accel += VECTOR3F(0, gravity, 0);
		break;
	}
	velocity += accel;
	mCharacter->SetChangState(false);
	float speed = sqrt(velocity.z * velocity.z);
	if (gravity <= mParameter.gravity*3.f)
	{
		gravity = mParameter.gravity;
	}
	if (speed > mParameter.maxSpeed)
	{
		velocity.z = velocity.z / speed * mParameter.maxSpeed;
	}
	position += velocity*elapsd_time;
	mCharacter->SetPosition(position);
	mCharacter->SetVelocity(velocity);
	Judgment::Judge(mCharacter.get(), manager);
	pCamera.GetCamera()->SetEye(mCharacter->GetPosition() + VECTOR3F(600, 250, -600));
	pCamera.GetCamera()->SetFocus(mCharacter->GetPosition());
	mCharacter->CalculateBoonTransform(elapsd_time);
	//mCharacter->CalculateBoonTransform(elapsd_time);
	//pCamera.GetCamera()->SetEye(position + VECTOR3F(600, 250, -600));
	//pCamera.GetCamera()->SetFocus(position);
}
