#pragma once
#include"character.h"

class PlayerCharacter :public Character
{
public:
	PlayerCharacter(std::shared_ptr<ModelResource>resouce);
	//�ړ�
	void Move(float elapsd_time);
	//state
	enum class MOVESTATE
	{
		MOVE,//�̏�̈ړ�
		JUMP,//�W�����v
		RAMP,//�W�����v��
		LANDING,//���n
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
	void SetBeforePosition(const VECTOR3F& beforePosition) { mBeforePosition = beforePosition; }
	//getter
	const VECTOR3F& GetVelocity() { return mVelocity; }
	const VECTOR3F& GetAccel() { return mAccel; }
	const bool GetExist() { return mExist; }
	const MOVESTATE&GetMoveState() { return mMoveState; }
	const bool GetChangState() { return mChangState; }
	const float GetMaxSpeed() { return mMaxSpeed; }
	const VECTOR3F& GetBeforePosition() { return mBeforePosition; }
private:
	VECTOR3F mVelocity;
	VECTOR3F mAccel;
	VECTOR3F mBeforePosition;
	bool mExist;
	MOVESTATE mMoveState;
	bool mChangState;
	float mMaxSpeed;
	float mMinSpeed;
};