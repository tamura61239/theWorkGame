#pragma once
#include"sprite.h"
#include<memory>
#include"vector.h"
#include<string>

class UI
{
public:
	//コンストラクタ
	UI(ID3D11Device* device, const wchar_t* textureName,const VECTOR2F&textureSize,const char*name);
	UI(){}
	//UIデータ
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
	//更新
	virtual void Update(){}
	//描画
	virtual void Render(ID3D11DeviceContext* context);
protected:
	//UIデータ
	UIData mData;
	//名前
	std::string mUIName;
	//画像
	std::unique_ptr<Sprite>mSprite;
};