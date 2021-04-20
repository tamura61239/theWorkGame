#include "select_text.h"

/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�`��֐�
/*****************************************************/
void SelectText::Render(ID3D11DeviceContext* context, Sprite* sprite, const VECTOR2F& position, const VECTOR2F& scale, const VECTOR2F& size, const VECTOR4F& color)
{
	//�f�[�^�����邩�ǂ���
	if (mTextureData.expired())return;
	if (mLocalData.expired())return;
	//�`��ɕK�v�ȃf�[�^�̎擾
	VECTOR2F leftTop = position + mLocalData.lock()->mPosition * size * scale;
	VECTOR2F textureSize = GetTextureSize();
	VECTOR2F drowSize = mLocalData.lock()->mScale * textureSize * scale;
	//�`��
	sprite->Render(context, mTextureData.lock()->mSRV.Get(), leftTop, drowSize, mUV, textureSize, 0, mDefColor * color * VECTOR4F(1, 1, 1, mLocalData.lock()->mAlpha));
}
