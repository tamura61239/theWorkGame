#pragma once
#include"character.h"

class PlayerCharacter :public Character
{
public:
	//�R���X�g���N�^
	PlayerCharacter(std::shared_ptr<ModelResource>resouce);
	//�ړ�
	void Move(float elapsd_time);
	//�A�j���[�V�����X�V
	void AnimUpdate(float elapsdTime);
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
	{//������ω��������̏���
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
	/**********�ϐ�*********/

	//���x
	VECTOR3F mVelocity;
	//�����x
	VECTOR3F mAccel;
	//�O�̃t���[���̍��W
	VECTOR3F mBeforePosition;
	//���݃t���O
	bool mExist;
	//���̏��
	MOVESTATE mMoveState;
	//��Ԃ��ς�������ǂ�����flag
	bool mChangState;
	//�󒆂łȂ����ǂ���
	bool mGroundFlag;
	//�S�[���������ǂ���
	bool mGorlFlag;
	//�ő呬�x
	float mMaxSpeed;
	//�ŏ����x
	float mMinSpeed;
	//�A�j���[�V�������x
	float mAnimSpeed;
	//�A�j���[�V����No
	int mAnimNo;
};