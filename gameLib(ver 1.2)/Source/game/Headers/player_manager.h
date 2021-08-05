#pragma once
#include"player_character.h"
#include"player_ai.h"
#include"model_renderer.h"
#include"stage_manager.h"

class PlayerManager
{
public:
	//コンストラクタ
	PlayerManager(ID3D11Device* device, const char* fileID);
	//エディタ関数
	void Editor();
	//更新
	void Update(float elapsdTime,StageManager*stageManager);
	//リスポン
	void Respond();
	//描画
	void Render(ID3D11DeviceContext* context,int type);
	enum RENDER_TYPE
	{
		COLOR,
		SHADOW,
		VELOCITY
	};
	//getter
	PlayerCharacter* GetCharacter() { return mCharacter.get(); }
	bool GetPlayFlag() { return mPlayFlag; }
	//setter
	void SetPlayFlag(const bool playFlag) { mPlayFlag = playFlag; }
private:
	//速度マップの描画
	void RenderVelocity(ID3D11DeviceContext* context);
	//シャドウマップの描画
	void RenderShadow(ID3D11DeviceContext* context);
	//変数
	std::unique_ptr<PlayerCharacter>mCharacter;
	std::unique_ptr<PlayerAI>mPlayer;
	std::unique_ptr<ModelRenderer>mRender;
	bool mPlayFlag;
};