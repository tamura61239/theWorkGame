#include "player_character.h"

PlayerCharacter::PlayerCharacter(std::shared_ptr<ModelResource> resouce):Character(resouce),mVelocity(0,0,0),mAccel(0,0,0),mExist(true)
,mMoveState(MOVESTATE::MOVE),mChangState(false), mMaxSpeed(0),mMinSpeed(0)
{
	mModel->PlayAnimation(0, true);
#ifdef USE_IMGUI
	mMoveState = MOVESTATE::EDITOR;
#endif
}

void PlayerCharacter::Move(float elapsd_time)
{
	mVelocity += mAccel;

	DirectX::XMVECTOR velocity = DirectX::XMLoadFloat3(&VECTOR3F(mVelocity.x, 0, mVelocity.z));
	float speed = 0;
	DirectX::XMStoreFloat(&speed, DirectX::XMVector3Length(velocity));
	if (speed > mMinSpeed)
	{
		VECTOR3F v = mVelocity / speed;
		if (speed > mMaxSpeed)
		{
			mVelocity = v * mMaxSpeed;
		}
		else
		{
			mVelocity -= v * mMinSpeed;
		}
	}
	else
	{
		mVelocity = VECTOR3F(0, 0, 0);
	}
	mPosition += mVelocity* elapsd_time;
	CalculateBoonTransform(elapsd_time);
}
