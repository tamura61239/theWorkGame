#pragma once
#include"tutorial_state.h"
#include"fade.h"
#include"player_manager.h"
#include"zoom_blur_parameter.h"

class SceneGameMove
{
public:
	//コンストラクタ
	SceneGameMove(ID3D11Device* device, int stageNo);
	//ズームブラーのエディタ
	void ZoomBlurEditor();
	//更新
	float Update(float elapsdTime, PlayerManager* player,bool testGame,bool editorFlag);
	//描画
	void Render(ID3D11DeviceContext* context, ID3D11ShaderResourceView* srv,Sprite*render);
	//getter
	TutorialState* GetTutorial() { return mTutorial.get(); }
	ConstantBuffer<CbZoom>* GetCbZoomBuffer() { return mCbZoomBlurBuffer.get(); }
private:
	//シーンの進行関数
	float SceneEnd(float elapsdTime);
	float SceneStart(float elapsdTime, bool editorFlag);
	float SceneMove(float elapsdTime, PlayerManager* player, bool testGame);
	//変数
	std::unique_ptr<TutorialState>mTutorial;
	std::unique_ptr<Fade>mFade;
	std::unique_ptr<ConstantBuffer<CbZoom>>mCbZoomBlurBuffer;
	std::unique_ptr<DrowShader>mZoomBlurShader;
	bool mGorlFlag;
	int mStageNo;
};