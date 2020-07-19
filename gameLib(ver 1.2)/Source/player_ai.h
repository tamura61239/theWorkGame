#pragma once
#include"player_character.h"

class PlayerAI
{
public:
	PlayerAI(ID3D11Device* device, const char* fileName);
	//load
	void Load();
	//save
	void Save();
	//imgui
	void ImGuiUpdate();
	//çXêV
	void Update(float elapsd_time);
	std::shared_ptr<PlayerCharacter>GetCharacter() { return mCharacter; }
private:
	std::shared_ptr<PlayerCharacter>mCharacter;
	struct PlayerParameter
	{
		VECTOR3F accel;
		float maxSpeed;
		float minSpeed;
	};
	PlayerParameter mParameter;
};