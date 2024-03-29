#pragma once
#include"stage_obj.h"
#include<vector>
#include"stage_editor.h"
#include"stage_operation.h"

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
	void Update(float elapsd_time, bool playFlag);
	//描画
	void Render(ID3D11DeviceContext* context);
	void Render(ID3D11DeviceContext* context,DrowShader*srv);
	//速度マップ用
	void RenderVelocity(ID3D11DeviceContext* context);
	//影用
	void RenderShadow(ID3D11DeviceContext* context);
	//横から見た時のシーン
	void SidoViewRender(ID3D11DeviceContext* context);
	//setter
	void SetStageNo(int no) { mStageNo = no; }
	//getter
	std::vector<std::shared_ptr<StageObj>>GetStages() { return mStageObjs; }
	StageEditor* GetStageEditor() { return mEditor.get(); }
	StageOperation* GetStageOperation() { return mOperation.get(); }
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
	std::vector<std::shared_ptr<StageObj>>mReds;
	std::vector<std::shared_ptr<StageObj>>mBlues;
	std::unique_ptr<StageOperation>mOperation;
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