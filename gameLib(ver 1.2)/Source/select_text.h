#pragma once
#include"vector.h"
#include<d3d11.h>
#include<wrl.h>
#include<memory>
#include<string>
#include"sprite.h"
//描画するテクスチャのデータ
struct TextureData
{
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mSRV;
	D3D11_TEXTURE2D_DESC mDesc;
	VECTOR2F mTextureDrowSize;
};
//ボード内での配置
struct LocalData
{
	VECTOR2F mPosition;
	VECTOR2F mScale;
	float mAlpha;
};
//ボード内に配置してあるテクスチャのクラス
class SelectText
{
public:
	//コンストラクタ
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
	//描画
	void Render(ID3D11DeviceContext* context, Sprite* sprite, const VECTOR2F& position, const VECTOR2F& scale,const VECTOR2F&size, const VECTOR4F& color);
private:
	//配置データ変数
	std::weak_ptr<LocalData>mLocalData;
	//画像データ変数
	std::weak_ptr<TextureData>mTextureData;
	//座標データ変数
	VECTOR2F mUV;
	VECTOR4F mDefColor;
	VECTOR2F mTextureSize;
};
