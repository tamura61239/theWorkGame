#pragma once
#include"scene.h"
#include"stage_select.h"
#include"bloom.h"

//セレクトシーン
class SceneSelect :public Scene
{
public:
	//コンストラクタ
	SceneSelect(){}
	//初期化
	void Initialize(ID3D11Device* device);
	//ImGuiでパラメーターを調整したりする用の関数
	void Editor();
	//更新
	void Update(float elapsed_time);
	//描画
	void Render(ID3D11DeviceContext* context, float elapsed_time);
	//解放
	void Relese();

private:
	//描画用変数
	std::unique_ptr<Sprite>mRenderTexture;
	std::unique_ptr<FrameBuffer>mColorMap;
	std::unique_ptr<FrameBuffer>mSceneFrame;
	std::unique_ptr<BloomRender>mBloom;
	//ステージを選ぶのに使う変数
	std::unique_ptr<StageSelect>mSelect;
};