#include "ui.h"

UI::UI(ID3D11Device* device, const wchar_t* textureName, const VECTOR2F& textureSize, const char* name)
{
	mSprite = std::make_unique<Sprite>(device, textureName);
	mData.mColor = VECTOR4F(1, 1, 1, 1);
	mData.mTextureSize = textureSize;
	mData.mLeftPosition = VECTOR2F(0, 0);
	mUIName = name;
	mData.mDrowSize = textureSize;
	mData.mAngle = 0;
	mData.mTextureLeftTop = VECTOR2F(0, 0);
}

void UI::Render(ID3D11DeviceContext* context)
{
	mSprite->Render(context, mData.mLeftPosition, mData.mDrowSize, mData.mTextureLeftTop, mData.mTextureSize, mData.mAngle, mData.mColor);
}
