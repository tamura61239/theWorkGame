#pragma once
#include"character.h"

class PlayerCharacter :public Character
{
public:
	//コンストラクタ
	PlayerCharacter(std::shared_ptr<ModelResource>resouce);
	//移動
	void Move(float elapsd_time);
	//アニメーション更新
	void AnimUpdate(float elapsdTime);
	//state
	enum class MOVESTATE
	{
		MOVE,//板の上の移動
		JUMP,//ジャンプ
		RAMP,//ジャンプ台
		LANDING,//着地
		MAX
	};
	//setter
	void SetVelocity(const VECTOR3F& velocity) { mVelocity = velocity; }
	void SetAccel(const VECTOR3F& accel) { mAccel = accel; }
	void SetExist(const bool exist) { mExist = exist; }
	void SetMoveState(MOVESTATE state)
	{//上程が変化した時の処理
		if (mMoveState == state)return;
		mChangState = true;
		mMoveState = state;
	}
	void SetChangState(const bool state) { mChangState = state; }
	void SetGroundFlag(const bool flag) { mGroundFlag = flag; }
	void SetGorlFlag(const bool flag) { mGorlFlag = flag; }
	void SetMaxSpeed(const float maxSpeed) { mMaxSpeed = maxSpeed; }
	void SetMinSpeed(const float minSpeed) { mMinSpeed = minSpeed; }
	void SetBeforePosition(const VECTOR3F& beforePosition) { mBeforePosition = beforePosition; }
	//getter
	const VECTOR3F& GetVelocity() { return mVelocity; }
	const VECTOR3F& GetAccel() { return mAccel; }
	const bool GetExist() { return mExist; }
	const MOVESTATE&GetMoveState() { return mMoveState; }
	const bool GetChangState() { return mChangState; }
	const bool GetGroundFlag() { return mGroundFlag; }
	const bool GetGorlFlag() { return mGorlFlag; }
	
	const float GetMaxSpeed() { return mMaxSpeed; }
	const VECTOR3F& GetBeforePosition() { return mBeforePosition; }
private:
	/**********変数*********/

	//速度
	VECTOR3F mVelocity;
	//加速度
	VECTOR3F mAccel;
	//前のフレームの座標
	VECTOR3F mBeforePosition;
	//存在フラグ
	bool mExist;
	//今の状態
	MOVESTATE mMoveState;
	//状態が変わったかどうかのflag
	bool mChangState;
	//空中でないかどうか
	bool mGroundFlag;
	//ゴールしたかどうか
	bool mGorlFlag;
	//最大速度
	float mMaxSpeed;
	//最小速度
	float mMinSpeed;
	//アニメーション速度
	float mAnimSpeed;
	//アニメーションNo
	int mAnimNo;
};