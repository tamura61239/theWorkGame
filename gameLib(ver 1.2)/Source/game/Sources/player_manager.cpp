#include "player_manager.h"
#include"Judgment.h"
#include"camera_manager.h"

/*****************************************************/
//　　　　　　　　　　初期化関数(コンストラクタ)
/*****************************************************/
PlayerManager::PlayerManager(ID3D11Device* device, const char* fileID)
{
	//モデルの描画用クラスの生成
	mRender = std::make_unique<ModelRenderer>(device);
	//キャラクターのモデルクラスの生成
	std::unique_ptr<ModelData>data = std::make_unique<ModelData>(fileID);
	std::shared_ptr<ModelResource>resouce = std::make_shared<ModelResource>(device, std::move(data));
	mCharacter = std::make_unique<PlayerCharacter>(resouce);
	mCharacter->SetColor(VECTOR4F(0.5, 0.5, 0.5, 1));
	//プレイヤーの行動制御クラス生成
	mPlayer = std::make_unique<PlayerAI>(mCharacter.get());
}
/*****************************************************/
//　　　　　　　　　　エディタ関数
/*****************************************************/

void PlayerManager::Editor()
{
	mPlayer->Editor(mCharacter.get());
}

/*****************************************************/
//　　　　　　　　　　更新関数
/*****************************************************/

void PlayerManager::Update(float elapsdTime, StageManager*stageManager)
{
	//プレイヤーが動いているとき
	if (mPlayFlag)
	{
		//レスポン処理の条件を満たしたとき
		if (mCharacter->GetPosition().y < -1000)
		{
			if (mCharacter->GetExist())
			{
				mCharacter->SetExist(false);
				stageManager->GetStageOperation()->Reset();
			}
			return;
		}
		//プレイヤーの行動の更新
		mPlayer->Update(elapsdTime, mCharacter.get());
		mCharacter->Move(elapsdTime);
	}
	else
	{
		Respond();
	}
	//当たり判定
	Judgment::Judge(mCharacter.get(), stageManager);
	//カメラにプレイヤーの座標を送信
	pCameraManager->GetCameraOperation()->GetPlayCamera()->SetPlayerPosition(mCharacter->GetPosition());
	//アニメーションの更新
	mCharacter->AnimUpdate(elapsdTime);
}
/********************リスポン時のプレイヤーのデータ更新***********************/
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
//　　　　　　　　　　描画関数
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
/********************速度マップの描画***********************/

void PlayerManager::RenderVelocity(ID3D11DeviceContext* context)
{
	mRender->VelocityBegin(context);
	mRender->VelocityDraw(context, mCharacter->GetModel());
	mRender->VelocityEnd(context);
}
/********************シャドウマップの描画***********************/

void PlayerManager::RenderShadow(ID3D11DeviceContext* context)
{
	mRender->ShadowBegin(context);
	mRender->ShadowDraw(context, mCharacter->GetModel());
	mRender->ShadowEnd(context);
}
