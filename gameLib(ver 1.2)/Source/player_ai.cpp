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
	ImGui::SliderFloat3("accel", *accel, -10, 10);
	ImGui::SliderFloat("maxSpeed", &mParameter.maxSpeed, mParameter.minSpeed, 10);
	ImGui::SliderFloat("minSpeed", &mParameter.minSpeed, 0, mParameter.maxSpeed);
	ImGui::End();
#endif
}

void PlayerAI::Update(float elapsd_time)
{
	ImGuiUpdate();

	mCharacter->Move(elapsd_time);
}
