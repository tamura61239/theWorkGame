#include "player_character.h"

/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�������֐�(�R���X�g���N�^)
/*****************************************************/
PlayerCharacter::PlayerCharacter(std::shared_ptr<ModelResource> resouce):Character(resouce),mVelocity(0,0,0),mAccel(0,0,0),mExist(true)
,mMoveState(MOVESTATE::LANDING),mChangState(false), mMaxSpeed(0),mMinSpeed(0),mGroundFlag(false), mGorlFlag(false), mAnimSpeed(0.f), mAnimNo(-1)
{
	mModel->PlayAnimation(0, true);
}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�X�V�֐�
/*****************************************************/

/********************�ړ�**************************/
void PlayerCharacter::Move(float elapsd_time)
{
	//�O�̃t���[���̍��W���擾
	mBeforePosition = mPosition;
	//���x�ɉ����x�𑫂�
	mVelocity += mAccel * elapsd_time;
	//���x��float�^�Ŏ擾
	float speed;
	DirectX::XMVECTOR velocity = DirectX::XMLoadFloat3(&VECTOR3F(0,0,mVelocity.z));
	DirectX::XMStoreFloat(&speed, DirectX::XMVector3Length(velocity));
	//�ő呬�x�ȏォ���ׂ�
	if (speed > mMaxSpeed)
	{
		mVelocity.z = mMaxSpeed;
	}
	//�Q�[���̃I�u�W�F�N�g�̑S�̂̃T�C�Y�ɍ��킹�������ɂ���
	static float s = mScale.x / 10;
	//���W���X�V
	mPosition += mVelocity * elapsd_time * s;
}
/********************�A�j���[�V�����X�V***********************/
void PlayerCharacter::AnimUpdate(float elapsdTime)
{
	//player�̏�Ԃ��ω��������ǂ���
	if (mChangState)
	{
		float blendTime = 0.f;
		//��Ԃɍ��킹�ăA�j���[�V����No�ƃA�j���[�V�������x�����߂�
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
		//�A�j���[�V�����J�n
		mModel->PlayAnimation(mAnimNo, true, blendTime);
		//flag��false�ɂ��Ă���
		mChangState = false;
	}
	//�A�j���[�V�����X�V
	CalculateBoonTransform(elapsdTime * mAnimSpeed);
}
