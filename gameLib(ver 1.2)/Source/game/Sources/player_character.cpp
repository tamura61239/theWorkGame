#include "player_character.h"

/*****************************************************/
//　　　　　　　　　　初期化関数(コンストラクタ)
/*****************************************************/
PlayerCharacter::PlayerCharacter(std::shared_ptr<ModelResource> resouce):Character(resouce),mVelocity(0,0,0),mAccel(0,0,0),mExist(true)
,mMoveState(MOVESTATE::LANDING),mChangState(false), mMaxSpeed(0),mMinSpeed(0),mGroundFlag(false), mGorlFlag(false), mAnimSpeed(0.f), mAnimNo(-1)
{
	mModel->PlayAnimation(0, true);
}
/*****************************************************/
//　　　　　　　　　　更新関数
/*****************************************************/

/********************移動**************************/
void PlayerCharacter::Move(float elapsd_time)
{
	//前のフレームの座標を取得
	mBeforePosition = mPosition;
	//速度に加速度を足す
	mVelocity += mAccel * elapsd_time;
	//速度をfloat型で取得
	float speed;
	DirectX::XMVECTOR velocity = DirectX::XMLoadFloat3(&VECTOR3F(0,0,mVelocity.z));
	DirectX::XMStoreFloat(&speed, DirectX::XMVector3Length(velocity));
	//最大速度以上か調べる
	if (speed > mMaxSpeed)
	{
		mVelocity.z = mMaxSpeed;
	}
	//ゲームのオブジェクトの全体のサイズに合わせた割合にする
	static float s = mScale.x / 10;
	//座標を更新
	mPosition += mVelocity * elapsd_time * s;
}
/********************アニメーション更新***********************/
void PlayerCharacter::AnimUpdate(float elapsdTime)
{
	//playerの状態が変化したかどうか
	if (mChangState)
	{
		float blendTime = 0.f;
		//状態に合わせてアニメーションNoとアニメーション速度を決める
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
		//アニメーション開始
		mModel->PlayAnimation(mAnimNo, true, blendTime);
		//flagをfalseにしておく
		mChangState = false;
	}
	//アニメーション更新
	CalculateBoonTransform(elapsdTime * mAnimSpeed);
}
