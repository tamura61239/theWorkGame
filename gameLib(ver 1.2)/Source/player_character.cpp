#include "player_character.h"

PlayerCharacter::PlayerCharacter(std::shared_ptr<ModelResource> resouce):Character(resouce),mVelocity(0,0,0),mAccel(0,0,0),mExist(true)
,mMoveState(MOVESTATE::LANDING),mChangState(false), mMaxSpeed(0),mMinSpeed(0)
{
	mModel->PlayAnimation(0, true);
}

void PlayerCharacter::Move(float elapsd_time)
{
}
