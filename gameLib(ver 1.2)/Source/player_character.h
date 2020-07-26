#pragma once
#include"character.h"

class PlayerCharacter :public Character
{
public:
	PlayerCharacter(std::shared_ptr<ModelResource>resouce);
	//移動
	void Move(float elapsd_time);
	//state
	enum class MOVESTATE
	{
		MOVE,//板の上の移動
		JUMP,//ジャンプ
		RAMP,//ジャンプ台
		LANDING,//着地
		EDITOR,//エディターモード
		MAX
	};
	//setter
	void SetVelocity(const VECTOR3F& velocity) { mVelocity = velocity; }
	void SetAccel(const VECTOR3F& accel) { mAccel = accel; }
	void SetExist(const bool exist) { mExist = exist; }
	void SetMoveState(MOVESTATE state)
	{
		mChangState = true;
		mMoveState = state;
	}
	void SetChangState(const bool state) { mChangState = state; }
	void SetMaxSpeed(const float maxSpeed) { mMaxSpeed = maxSpeed; }
	void SetMinSpeed(const float minSpeed) { mMinSpeed = minSpeed; }
	//getter
	const VECTOR3F& GetVelocity() { return mVelocity; }
	const VECTOR3F& GetAccel() { return mAccel; }
	const bool GetExist() { return mExist; }
	const MOVESTATE&GetMoveState() { return mMoveState; }
	const bool GetChangState() { return mChangState; }
	const float GetMaxSpeed() { return mMaxSpeed; }
private:
	VECTOR3F mVelocity;
	VECTOR3F mAccel;
	bool mExist;
	MOVESTATE mMoveState;
	bool mChangState;
	float mMaxSpeed;
	float mMinSpeed;
};