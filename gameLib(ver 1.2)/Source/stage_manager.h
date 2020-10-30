#pragma once
#include"stage_obj.h"
#include<vector>
#include"stage_editor.h"

class StageManager
{
public:
	StageManager(ID3D11Device* device, int width, int height);
	static void StageCount();
	void Clear() { mStageObjs.clear(); }
	void Load();
	void Save();
	void ImGuiUpdate();
	void Update(float elapsd_time);
	void Render(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection, const int stageState,DrowShader*srv=nullptr);
	void RenderVelocity(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection,const int stageState);
	void SidoViewRender(ID3D11DeviceContext* context);
	void SetStageNo(int no) { stageNo = no; }
	//getter
	std::vector<std::shared_ptr<StageObj>>GetStages() { return mStageObjs; }
	StageEditor* GetStageEditor() { return mEditor.get(); }
	static const int GetMaxStageCount() { return mMaxStage; }
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbBeforeBuffer;
	std::unique_ptr<DrowShader>mVelocityShader;
	std::unique_ptr<DrowShader>mDeferredShader;
	
	static int mMaxStage;

	std::vector < std::shared_ptr<StageObj>>mStageObjs;
	std::unique_ptr<MeshRender>mRender;
	std::unique_ptr<StageEditor>mEditor;
	int stageNo;
	std::vector<std::shared_ptr<StaticMesh>>mMeshs;
	//editorïœêî
	float mWidth;
	float mHeight;
	VECTOR3F mNearMouse;
	VECTOR3F mFarMouse;
	int dragObjNumber;
};