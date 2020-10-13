#pragma once
#include"sprite.h"
#include<memory>
#include"vector.h"
#include<string>

class UI
{
public:
	UI(ID3D11Device* device, const wchar_t* textureName,const VECTOR2F&textureSize,const char*name);
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
	const UIData GetUIData() { return mData; }
	const std::string GetName() { return mUIName; }
	//•`‰æ
	void Render(ID3D11DeviceContext* context);
private:
	UIData mData;
	std::string mUIName;
	std::unique_ptr<Sprite>mSprite;
};