#pragma once
#include"vector.h"
#include"sprite.h"
#include"player_character.h"
#include<memory>
#include"constant_buffer.h"
#include"zoom_blur_parameter.h"

class TutorialState
{
public:
	TutorialState(ID3D11Device*device);
	float Update(float elapsdTime, PlayerCharacter* player);
	void ImGuiUpdate();
	void RenderButton(ID3D11DeviceContext* context);
	void RenderText(ID3D11DeviceContext* context);
	const VECTOR4F& GetBackGroundColor() { return mBackGroundColor; }
	const int& GetState() { return mState; }
	void ResetParameter()
	{
		mState = 0;
		mBackGroundColor = VECTOR4F(1, 1, 1, 1);
		mTextAlpha = 1;
		mCount = 0;
		mTimer = 0;
	}
	ConstantBuffer<CbZoom>* GetCbZoom() { return mCbZoom.get(); }
	bool GetKeyFlag() {
		return mKeyFlag;
	}
private:
	//�ŏ��̃W�����v���������ǂ����𔻒f����֐�
	void CheckFirstJump(PlayerCharacter* player);
	//space�L�[�������܂ŃQ�[���̐i�s���~�߂Ƃ��֐�
	float StopScene(float elapsdTime);
	//���̐������������������������ǂ���
	void CheckNextExplanation(float elapsdTime, PlayerCharacter* player);
	//�������\�����I�����邩�ǂ���
	float CheckEndExplanation(float elapsdTime);
	int mState;
	VECTOR4F mBackGroundColor;
	float mTextAlpha;
	int mCount;
	float mTimer;
	float mLockTime;
	float mNextTime;
	bool mKeyFlag;
	VECTOR2F mTextPosition;
	VECTOR2F mTextSize;
	float mEasingTimer;
	std::unique_ptr<ConstantBuffer<CbZoom>>mCbZoom;
	std::unique_ptr<Sprite>mRender;
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>mTexts;
};