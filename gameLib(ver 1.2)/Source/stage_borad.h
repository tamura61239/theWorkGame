#pragma once
#include"select_text.h"
#include<vector>

//�X�e�[�W�̃{�[�h�N���X
class StageBorad
{
public:
	//�R���X�g���N�^
	StageBorad():mPosition(0,0),mScale(1,1),mColor(1,1,1,1),mStageNo(0), mInterval(0), mSize(0,0){}
	//�e�L�X�g����
	void CreateText(std::shared_ptr<TextureData>textureData, std::shared_ptr<LocalData>localData,const VECTOR2F&uv,const VECTOR4F&color=VECTOR4F(1,1,1,1));
	//getter
	std::vector<std::shared_ptr<SelectText>>GetTextes() { return mTexts; }
	const int GetStageNo() { return mStageNo; }
	//setter
	void SetStageNo(const int number) { mStageNo = number; }
	void SetSize(const VECTOR2F& size) { mSize = size; }
	void SetInterval(const float interval) { mInterval = interval; }
	//�X�V
	void Update(int changeSelect,float elapsdTime,int nowStageNo,const VECTOR2F&centerPosition);
	//�`��
	void Render(ID3D11DeviceContext* context, Sprite* sprite);
private:
	std::vector<std::shared_ptr<SelectText>>mTexts;
	//���W
	VECTOR2F mPosition;
	//�X�P�[��
	VECTOR2F mScale;
	//�{�[�h�̃T�C�Y
	VECTOR2F mSize;
	//�F
	VECTOR4F mColor;
	//�X�e�[�W�ԍ�
	int mStageNo;
	//�{�[�h�̕�
	float mInterval;
};