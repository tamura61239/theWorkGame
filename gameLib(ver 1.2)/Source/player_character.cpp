#include "player_character.h"

PlayerCharacter::PlayerCharacter(std::shared_ptr<ModelResource> resouce):Character(resouce),mVelocity(0,0,0),mAccel(0,0,0),mExist(true)
,mMoveState(MOVESTATE::LANDING),mChangState(false), mMaxSpeed(0),mMinSpeed(0),mGroundFlag(false)
{
	mModel->PlayAnimation(0, true);
}

void PlayerCharacter::Move(float elapsd_time)
{
	mBeforePosition = mPosition;
	mChangState = false;
	mVelocity += mAccel * elapsd_time;
	float speed = sqrtf(mVelocity.z * mVelocity.z);
	if (speed > mMaxSpeed)
	{
		mVelocity.z =  mMaxSpeed;
	}
	static float s = mScale.x / 10;
	mPosition += mVelocity * elapsd_time * s;
}
