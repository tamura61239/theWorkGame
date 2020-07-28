#include "player_ai.h"
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
	float* j[3] = { &jump.x,&jump.y,&jump.z };
	ImGui::SliderFloat3("jump", *j, 0, 500);
	float* r[3] = { &ranp.x,&ranp.y,&ranp.z };
	ImGui::SliderFloat3("ranp", *r, 0, 500);
	if (ImGui::SliderFloat("maxSpeed", &mParameter.maxSpeed, mParameter.minSpeed, 500))
	{
		mCharacter->SetMaxSpeed(mParameter.maxSpeed);
	}
	if (ImGui::SliderFloat("minSpeed", &mParameter.minSpeed, 0, mParameter.maxSpeed))
	{
		mCharacter->SetMinSpeed(mParameter.minSpeed);
	}
	if (ImGui::Button("save"))
	{
		Save();
	}
	ImGui::Checkbox("play", &play);
	ImGui::Text("%f,%f,%f", mCharacter->GetPosition().x, mCharacter->GetPosition().y, mCharacter->GetPosition().z);
	ImGui::End();
#endif
}

void PlayerAI::Update(float elapsd_time)
{
	ImGuiUpdate();
	static float g = -9.8f*30;
	if (!play)return;
	if (mCharacter->GetPosition().y < -300)
	{
		play = false;
		mCharacter->SetPosition(VECTOR3F(0, 10, 0));
		mCharacter->SetVelocity(VECTOR3F(0, 0, 0));
		mCharacter->CalculateBoonTransform(0);
		g = -9.8f*30;
		return;
	}
	VECTOR3F position = mCharacter->GetPosition();
	mCharacter->SetBeforePosition(position);
	mCharacter->SetChangState(false);
	VECTOR3F velocity = mCharacter->GetVelocity();
	
	switch (mCharacter->GetMoveState())
	{
	case PlayerCharacter::MOVESTATE::MOVE:
		g = -9.8f * 30;
		velocity.y = g*0.03f;
		velocity += mParameter.accel * elapsd_time;
		//velocity += VECTOR3F(0, g, 0) * elapsd_time;

		break;
	case PlayerCharacter::MOVESTATE::LANDING:
		g = g + g * elapsd_time;
		velocity += mParameter.accel * elapsd_time;
		velocity += VECTOR3F(0, g, 0) * elapsd_time;

		break;
	case PlayerCharacter::MOVESTATE::JUMP:
		velocity += mParameter.accel * elapsd_time;
		velocity += jump;
		mCharacter->SetMoveState(PlayerCharacter::MOVESTATE::LANDING);
		break;
	}
	float speed = sqrt(velocity.z * velocity.z);

	if (speed > mParameter.maxSpeed)
	{
		velocity.z = velocity.z / speed * mParameter.maxSpeed;
	}
	position += velocity*elapsd_time;
	mCharacter->SetPosition(position);
	mCharacter->SetVelocity(velocity);
	mCharacter->CalculateBoonTransform(elapsd_time);
}
