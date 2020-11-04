#include "select_text.h"

//void SelectText::Drow(ID3D11DeviceContext* context, Sprite* sprite, const VECTOR2F& position, const VECTOR2F& scale, const VECTOR4F& color)
//{
//	if (!mTexture.expired())return;
//	VECTOR4F lColor = color * VECTOR4F(1, 1, 1, mAlpha);
//	sprite->Render(context, mTexture.lock()->mSRV.Get(), position + mLocalPosition * scale, mLocalSize * scale, mUV, mTexture.lock()->mTextureDrowSize, 0, lColor);
//}

void SelectText::Render(ID3D11DeviceContext* context, Sprite* sprite, const VECTOR2F& position, const VECTOR2F& scale, const VECTOR2F& size, const VECTOR4F& color)
{
	if (mTextureData.expired())return;
	if (mLocalData.expired())return;
	VECTOR2F leftTop = position + mLocalData.lock()->mPosition * size * scale;
	VECTOR2F textureSize = GetTextureSize();
	VECTOR2F drowSize = mLocalData.lock()->mScale * textureSize * scale;
	sprite->Render(context, mTextureData.lock()->mSRV.Get(), leftTop, drowSize, mUV, textureSize, 0, mDefColor * color * VECTOR4F(1, 1, 1, mLocalData.lock()->mAlpha));
}
