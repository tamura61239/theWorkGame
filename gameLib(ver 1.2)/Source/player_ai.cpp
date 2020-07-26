#include "player_ai.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif
PlayerAI::PlayerAI(ID3D11Device* device, const char* fileName)
{
	std::unique_ptr<ModelData>data = std::make_unique<ModelData>(fileName);
	std::shared_ptr<ModelResource>resouce = std::make_shared<ModelResource>(device, std::move(data));
	mCharacter = std::make_shared<PlayerCharacter>(resouce);
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

void PlayerAI::ImGuiUpdate()
{
#ifdef USE_IMGUI
	ImGui::Begin("player");
	float* accel[3] = { &mParameter.accel.x,&mParameter.accel.y ,&mParameter.accel.z };
	ImGui::SliderFloat3("accel", *accel, -100, 100);
	if (ImGui::SliderFloat("maxSpeed", &mParameter.maxSpeed, mParameter.minSpeed, 100))
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
	ImGui::Text("%f,%f,%f", mCharacter->GetPosition().x, mCharacter->GetPosition().y, mCharacter->GetPosition().z);
	ImGui::End();
#endif
}

void PlayerAI::Update(float elapsd_time)
{
	ImGuiUpdate();
	switch (mCharacter->GetMoveState())
	{
	case PlayerCharacter::MOVESTATE::MOVE:
		mCharacter->SetAccel(mParameter.accel);
		break;
	}
	mCharacter->Move(elapsd_time);
}
