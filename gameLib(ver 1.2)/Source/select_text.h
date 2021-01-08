#pragma once
#include"vector.h"
#include<d3d11.h>
#include<wrl.h>
#include<memory>
#include<string>
#include"sprite.h"
struct TextureData
{
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mSRV;
	D3D11_TEXTURE2D_DESC mDesc;
	VECTOR2F mTextureDrowSize;
};
struct LocalData
{
	VECTOR2F mPosition;
	VECTOR2F mScale;
	float mAlpha;
};
class SelectText
{
public:
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
	void Render(ID3D11DeviceContext* context, Sprite* sprite, const VECTOR2F& position, const VECTOR2F& scale,const VECTOR2F&size, const VECTOR4F& color);
private:
	std::weak_ptr<LocalData>mLocalData;
	std::weak_ptr<TextureData>mTextureData;
	VECTOR2F mUV;
	VECTOR4F mDefColor;
	VECTOR2F mTextureSize;
};
//struct TextureData
//{
//	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>SRV;
//	D3D11_TEXTURE2D_DESC mDesc;
//	VECTOR2F mTextureDrowSize;
//};
//class SelectText
//{
//public:
//	SelectText() :mLocalPosition(0, 0), mLocalSize(10, 10), mUV(0, 0), mAlpha(0){}
//	void SetDrowPositionData(const VECTOR2F& position, const VECTOR2F& size, const float alpha)
//	{
//		mLocalPosition = position;
//		mLocalSize = size;
//		mAlpha = alpha;
//	}
//	void SetUV(const VECTOR2F& uv)
//	{
//		mUV = uv;
//	}
//	void SetTextureData(std::shared_ptr<TextureData>texture)
//	{
//		mTexture = texture;
//	}
//	void Drow(ID3D11DeviceContext* context, Sprite* sprite, const VECTOR2F& position, const VECTOR2F& scale, const VECTOR4F& color);
//private:
//	std::weak_ptr<TextureData>mTexture;
//	VECTOR2F mLocalPosition;
//	VECTOR2F mLocalSize;
//	VECTOR2F mUV;
//	float mAlpha;
//};