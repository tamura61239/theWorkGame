#include "player_manager.h"
#include"Judgment.h"
#include"camera_manager.h"

/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�������֐�(�R���X�g���N�^)
/*****************************************************/
PlayerManager::PlayerManager(ID3D11Device* device, const char* fileID)
{
	//���f���̕`��p�N���X�̐���
	mRender = std::make_unique<ModelRenderer>(device);
	//�L�����N�^�[�̃��f���N���X�̐���
	std::unique_ptr<ModelData>data = std::make_unique<ModelData>(fileID);
	std::shared_ptr<ModelResource>resouce = std::make_shared<ModelResource>(device, std::move(data));
	mCharacter = std::make_unique<PlayerCharacter>(resouce);
	mCharacter->SetColor(VECTOR4F(0.5, 0.5, 0.5, 1));
	//�v���C���[�̍s������N���X����
	mPlayer = std::make_unique<PlayerAI>(mCharacter.get());
}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�G�f�B�^�֐�
/*****************************************************/

void PlayerManager::Editor()
{
	mPlayer->Editor(mCharacter.get());
}

/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�X�V�֐�
/*****************************************************/

void PlayerManager::Update(float elapsdTime, StageManager*stageManager)
{
	//�v���C���[�������Ă���Ƃ�
	if (mPlayFlag)
	{
		//���X�|�������̏����𖞂������Ƃ�
		if (mCharacter->GetPosition().y < -1000)
		{
			if (mCharacter->GetExist())
			{
				mCharacter->SetExist(false);
				stageManager->GetStageOperation()->Reset();
			}
			return;
		}
		//�v���C���[�̍s���̍X�V
		mPlayer->Update(elapsdTime, mCharacter.get());
		mCharacter->Move(elapsdTime);
	}
	else
	{
		Respond();
	}
	//�����蔻��
	Judgment::Judge(mCharacter.get(), stageManager);
	//�J�����Ƀv���C���[�̍��W�𑗐M
	pCameraManager->GetCameraOperation()->GetPlayCamera()->SetPlayerPosition(mCharacter->GetPosition());
	//�A�j���[�V�����̍X�V
	mCharacter->AnimUpdate(elapsdTime);
}
/********************���X�|�����̃v���C���[�̃f�[�^�X�V***********************/
void PlayerManager::Respond()
{
	mPlayer->Reset();
	mCharacter->SetPosition(VECTOR3F(0, 10, 0));
	mCharacter->SetBeforePosition(VECTOR3F(0, 10, 0));
	mCharacter->SetVelocity(VECTOR3F(0, 0, 0));
	mCharacter->SetAngle(VECTOR3F(0, 0, 0));
	mCharacter->CalculateBoonTransform(0);
	mCharacter->SetAccel(VECTOR3F(0, 0, 0));
	mCharacter->SetExist(true);
	mCharacter->SetGorlFlag(false);
	mCharacter->SetGroundFlag(false);

}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�`��֐�
/*****************************************************/

void PlayerManager::Render(ID3D11DeviceContext* context, int type)
{
	if (!mCharacter->GetExist())return;
	switch (type)
	{
	case RENDER_TYPE::COLOR:
		mRender->Draw(context, mCharacter->GetModel(), mCharacter->GetColor());
		break;
	case RENDER_TYPE::SHADOW:
		RenderShadow(context);
		break;
	case RENDER_TYPE::VELOCITY:
		RenderShadow(context);
		break;
	}
}
/********************���x�}�b�v�̕`��***********************/

void PlayerManager::RenderVelocity(ID3D11DeviceContext* context)
{
	mRender->VelocityBegin(context);
	mRender->VelocityDraw(context, mCharacter->GetModel());
	mRender->VelocityEnd(context);
}
/********************�V���h�E�}�b�v�̕`��***********************/

void PlayerManager::RenderShadow(ID3D11DeviceContext* context)
{
	mRender->ShadowBegin(context);
	mRender->ShadowDraw(context, mCharacter->GetModel());
	mRender->ShadowEnd(context);
}
