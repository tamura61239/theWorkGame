#include "select_text.h"

/*****************************************************/
//　　　　　　　　　　描画関数
/*****************************************************/
void SelectText::Render(ID3D11DeviceContext* context, Sprite* sprite, const VECTOR2F& position, const VECTOR2F& scale, const VECTOR2F& size, const VECTOR4F& color)
{
	//データがあるかどうか
	if (mTextureData.expired())return;
	if (mLocalData.expired())return;
	//描画に必要なデータの取得
	VECTOR2F leftTop = position + mLocalData.lock()->mPosition * size * scale;
	VECTOR2F textureSize = GetTextureSize();
	VECTOR2F drowSize = mLocalData.lock()->mScale * textureSize * scale;
	//描画
	sprite->Render(context, mTextureData.lock()->mSRV.Get(), leftTop, drowSize, mUV, textureSize, 0, mDefColor * color * VECTOR4F(1, 1, 1, mLocalData.lock()->mAlpha));
}
