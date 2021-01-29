#include "player_character.h"

PlayerCharacter::PlayerCharacter(std::shared_ptr<ModelResource> resouce):Character(resouce),mVelocity(0,0,0),mAccel(0,0,0),mExist(true)
,mMoveState(MOVESTATE::LANDING),mChangState(false), mMaxSpeed(0),mMinSpeed(0),mGroundFlag(false), mGorlFlag(false), mAnimSpeed(0.f), mAnimNo(-1)
{
	mModel->PlayAnimation(0, true);
}

void PlayerCharacter::Move(float elapsd_time)
{
	mBeforePosition = mPosition;
	mVelocity += mAccel * elapsd_time;
	float speed = sqrtf(mVelocity.z * mVelocity.z);
	if (speed > mMaxSpeed)
	{
		mVelocity.z =  mMaxSpeed;
	}
	mChangState = false;

	static float s = mScale.x / 10;
	mPosition += mVelocity * elapsd_time * s;
}

void PlayerCharacter::AnimUpdate(float elapsdTime)
{
	if (mChangState)
	{
		float blendTime = 0.f;
		switch (mMoveState)
		{
		case MOVESTATE::MOVE:
		case MOVESTATE::LANDING:
			if (mAnimNo != 0)
			{
				mAnimNo = 0;
				mAnimSpeed = 2.0f;
				blendTime = 0.1f;
			}
			break;
		case MOVESTATE::JUMP:
		case MOVESTATE::RAMP:
			if (mAnimNo != 2)
			{
				mAnimNo = 2;
				mAnimSpeed = 4.0f;
				blendTime = 0.4f;
			}
			break;
		}
		mModel->PlayAnimation(mAnimNo, true, blendTime);
	}
	CalculateBoonTransform(elapsdTime * mAnimSpeed);
}
