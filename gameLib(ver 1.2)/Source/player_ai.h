#pragma once
#include"player_character.h"
#include"stage_manager.h"
#include"stage_operation.h"

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
	//setter
	void SetPlayFlag(const bool flag) { mPlayFlag = flag; }
	//getter
	const bool GetPlayFlag() { return mPlayFlag; }
	//çXêV
	void Update(float elapsd_time,StageManager*manager,StageOperation*operation);
	PlayerCharacter*GetCharacter() { return mCharacter.get(); }
private:
	std::unique_ptr<PlayerCharacter>mCharacter;
	struct PlayerParameter
	{
		VECTOR3F accel;
		float maxSpeed;
		float minSpeed;
		VECTOR3F jump;
		VECTOR3F ranp;
		float gravity;
	};
	PlayerParameter mParameter;
	bool mPlayFlag;
};