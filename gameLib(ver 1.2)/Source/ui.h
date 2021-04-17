#pragma once
#include"sprite.h"
#include<memory>
#include"vector.h"
#include<string>

class UI
{
public:
	//�R���X�g���N�^
	UI(ID3D11Device* device, const wchar_t* textureName,const VECTOR2F&textureSize,const char*name);
	UI(){}
	//UI�f�[�^
	struct UIData
	{
		VECTOR2F mLeftPosition;
		VECTOR2F mDrowSize;
		VECTOR4F mColor;
		VECTOR2F mTextureSize;
		VECTOR2F mTextureLeftTop;
		float mAngle;
	};
	//setter
	void SetUIData(const UIData&data) { mData = data; }
	//getter
	UIData GetUIData() { return mData; }
	const std::string GetName() { return mUIName; }
	//�X�V
	virtual void Update(){}
	//�`��
	virtual void Render(ID3D11DeviceContext* context);
protected:
	//UI�f�[�^
	UIData mData;
	//���O
	std::string mUIName;
	//�摜
	std::unique_ptr<Sprite>mSprite;
};