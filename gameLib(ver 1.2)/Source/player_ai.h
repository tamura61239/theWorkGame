#pragma once
#include"player_character.h"
#include"stage_manager.h"

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
	void Update(float elapsd_time,StageManager*manager);
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
	struct CameraParameter
	{
		float angle;
		float length;
		float y;
	};
	PlayerParameter mParameter;
	CameraParameter mCameraParameter;
};