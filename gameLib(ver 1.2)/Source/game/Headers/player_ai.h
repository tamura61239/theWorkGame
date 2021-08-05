#pragma once
#include"player_character.h"
#include"stage_manager.h"
#include"stage_operation.h"

class PlayerAI
{
public:
	//コンストラクタ
	PlayerAI(PlayerCharacter* character);
	//エディター
	void Editor(PlayerCharacter* character);
	//更新
	void Update(float elapsd_time,PlayerCharacter*character);
	//リスポン
	void Reset();
private:
	//エディター変数
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
	//重力
	float mGravity;
};