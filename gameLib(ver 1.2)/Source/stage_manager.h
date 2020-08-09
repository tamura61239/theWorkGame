#pragma once
#include"stage_obj.h"
#include<vector>
#include"stage_obj_drag_operation.h"

class StageManager
{
public:
	StageManager(ID3D11Device* device, int width, int height);
	void Load();
	void Save();
	void ImGuiUpdate();
	int CheckMouseDragObj();
	void Update(float elapsd_time);
	void Render(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection);
	void SetStageNo(int no) { stageNo = no; }
	//getter
	std::vector<std::shared_ptr<StageObj>>GetStages() { return mStageObjs; }
private:
	void ScreeenToWorld(VECTOR3F* worldPosition, const VECTOR3F& screenPosition);
	void MouseToWorld();

	std::vector < std::shared_ptr<StageObj>>mStageObjs;
	std::unique_ptr<MeshRender>mRender;
	int stageNo;
	std::vector<std::shared_ptr<StaticMesh>>mMeshs;
	std::shared_ptr<StageObjDragOperation>mDragOperation;
	float mWidth;
	float mHeight;
	VECTOR3F mNearMouse;
	VECTOR3F mFarMouse;
	int dragObjNumber;
};