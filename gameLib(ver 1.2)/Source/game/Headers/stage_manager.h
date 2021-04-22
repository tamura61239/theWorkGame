#pragma once
#include"stage_obj.h"
#include<vector>
#include"stage_editor.h"

class StageManager
{
public:
	//コンストラクタ
	StageManager(ID3D11Device* device, int width, int height);
	//ステージが全部でいくつあるかを調べる
	static void StageCount();
	//ステージのオブジェクトの生成
	void Clear() { mStageObjs.clear(); }
	//ファイル操作
	void Load();
	void Save();
	//エディタ
	void Editor();
	//更新
	void Update(float elapsd_time);
	//描画
	void Render(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection, const int stageState,DrowShader*srv=nullptr);
	//速度マップ用
	void RenderVelocity(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection, const int stageState);
	//影用
	void RenderShadow(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection);
	//横から見た時のシーン
	void SidoViewRender(ID3D11DeviceContext* context);
	//setter
	void SetStageNo(int no) { mStageNo = no; }
	//getter
	std::vector<std::shared_ptr<StageObj>>GetStages() { return mStageObjs; }
	StageEditor* GetStageEditor() { return mEditor.get(); }
	static const int GetMaxStageCount() { return mMaxStage; }
	const int GrtStageNo() { return mStageNo; }
private:
	//描画用変数
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbBeforeBuffer;
	std::unique_ptr<DrowShader>mVelocityShader;
	std::unique_ptr<DrowShader>mDeferredShader;
	std::unique_ptr<MeshRender>mRender;
	//ステージの最大数
	static int mMaxStage;
	//ステージのオブジェクト
	std::vector < std::shared_ptr<StageObj>>mStageObjs;
	//エディタ
	std::unique_ptr<StageEditor>mEditor;
	//ステージ番号
	int mStageNo;
	//ステージのオブジェクトの種類
	std::vector<std::shared_ptr<StaticMesh>>mMeshs;
	//editor変数
	float mWidth;
	float mHeight;
	VECTOR3F mNearMouse;
	VECTOR3F mFarMouse;
	int dragObjNumber;
};