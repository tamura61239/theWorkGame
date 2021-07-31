#pragma once
#include"player_character.h"
#include"stage_manager.h"
#include"stage_operation.h"

class PlayerAI
{
public:
	//コンストラクタ
	PlayerAI(ID3D11Device* device, const char* fileName);
	//エディター
	void Editor();
	//setter
	void SetPlayFlag(const bool flag) { mPlayFlag = flag; }
	//getter
	const bool GetPlayFlag() { return mPlayFlag; }
	PlayerCharacter* GetCharacter() { return mCharacter.get(); }
	//更新
	void Update(float elapsd_time,StageManager*manager,StageOperation*operation);
	//playerを動かす
	void Move(float elapsd_time);
	//リスポン
	void Respond();
private:
	//playerオブジェクト変数
	std::unique_ptr<PlayerCharacter>mCharacter;
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
	//プレイできるかどうか
	bool mPlayFlag;
};