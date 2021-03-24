#pragma once
#include"player_character.h"
#include"stage_manager.h"
#include"stage_operation.h"

class PlayerAI
{
public:
	PlayerAI(ID3D11Device* device, const char* fileName);
	//imgui
	void ImGuiUpdate();
	//setter
	void SetPlayFlag(const bool flag) { mPlayFlag = flag; }
	//getter
	const bool GetPlayFlag() { return mPlayFlag; }
	PlayerCharacter* GetCharacter() { return mCharacter.get(); }
	//çXêV
	void Update(float elapsd_time,StageManager*manager,StageOperation*operation);
	void Move(float elapsd_time);
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
	float mGravity;
	bool mPlayFlag;
};