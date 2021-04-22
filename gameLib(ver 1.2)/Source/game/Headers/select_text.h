#pragma once
#include"vector.h"
#include<d3d11.h>
#include<wrl.h>
#include<memory>
#include<string>
#include"sprite.h"
//�`�悷��e�N�X�`���̃f�[�^
struct TextureData
{
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mSRV;
	D3D11_TEXTURE2D_DESC mDesc;
	VECTOR2F mTextureDrowSize;
};
//�{�[�h���ł̔z�u
struct LocalData
{
	VECTOR2F mPosition;
	VECTOR2F mScale;
	float mAlpha;
};
//�{�[�h���ɔz�u���Ă���e�N�X�`���̃N���X
class SelectText
{
public:
	//�R���X�g���N�^
	SelectText():mUV(0,0),mDefColor(1,1,1,1){}
	//setter
	void SetLocalData(std::shared_ptr<LocalData>localData)
	{
		mLocalData = localData;
	}
	void SetTextureData(std::shared_ptr<TextureData>textureData)
	{
		mTextureData = textureData;
	}
	void SetUV(const VECTOR2F& uv)
	{
		mUV = uv;
	}
	void SetColor(const VECTOR4F& color)
	{
		mDefColor = color;
	}
	//getter
	const VECTOR2F& GetTextureSize() 
	{ 
		mTextureSize = mTextureData.expired() ? VECTOR2F(0, 0) : mTextureData.lock()->mTextureDrowSize;
		return  mTextureSize;
	}
	//�`��
	void Render(ID3D11DeviceContext* context, Sprite* sprite, const VECTOR2F& position, const VECTOR2F& scale,const VECTOR2F&size, const VECTOR4F& color);
private:
	//�z�u�f�[�^�ϐ�
	std::weak_ptr<LocalData>mLocalData;
	//�摜�f�[�^�ϐ�
	std::weak_ptr<TextureData>mTextureData;
	//���W�f�[�^�ϐ�
	VECTOR2F mUV;
	VECTOR4F mDefColor;
	VECTOR2F mTextureSize;
};
