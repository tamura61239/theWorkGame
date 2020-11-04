#pragma once
#include"stage_obj.h"
#include"framebuffer.h"
#include"sprite.h"
#include"camera.h"

class StageEditor
{
public:
	//コンストラクタ
	StageEditor(ID3D11Device*device,int width,int height);
	//ステージデータのロード
	void Clear()
	{
		mEditorFlag = false;
		mFileState = 0;
		mDragObjNo = -1;
	}
	void ClearCreateData();
	//getter
	FrameBuffer* GetStageSidoViewBuffer() { return mStageSidoView.get(); }
	Camera* GetCamera() { return mSidoCamera.get(); }
	const bool GetEditorFlag() { return mEditorFlag; }
	const bool GetCreateFlag() { return mCreateFlag; }
	const int GetFileState() { return mFileState; }
	const StageData GetCreateData() { return mCreateData; }
	const int GetDeleteNo() { return mDeleteNum; }
	const bool GetSceneSaveFlag() { return mSaveSceneFlag; }
	//clear
	void ClearFileState() { mFileState = 0; }
	//更新
	int Update(std::vector<std::shared_ptr<StageObj>>objs);
	//描画
	void Render(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection);
	void SidoViewRender(ID3D11DeviceContext* context);
	void EditorCreateObjImageRender(ID3D11DeviceContext* context, StaticMesh* mesh,MeshRender*render,VECTOR4F color);
private:
	//マウス更新関数
	void UpdateMouseData();
	/*********マウス座標変換関数**********/
	//スクリーン座標→ワールド座標
	void ScreeenToWorld(VECTOR3F* worldPosition, const VECTOR3F& screenPosition, Camera* camera);
	void MouseToWorld(Camera*camera,const VECTOR2F&mousePosition);
	//sidoカメラ関数
	void SetSidoCamera(std::vector<std::shared_ptr<StageObj>>objs);
	/*********マウスがクリックしているobj判定********/
	void SearchStageObj(std::vector<std::shared_ptr<StageObj>>objs);
	int NewDragObj(std::vector<std::shared_ptr<StageObj>>objs);
	//マウスの位置判定
	bool MouseJudg();
	//カメラの新しい座標計算
	void NewCameraPosition();
	//設定
	void CreateDataSetTransformData()
	{
		static float s = gameObjScale / 10.0f;
		mCreateData.mAngle = mObj->GetAngle();
		mCreateData.mScale = mObj->GetScale() / s;
		mCreateData.mPosition = mObj->GetPosition() / s;
		mCreateFlag = true;
	}
	//変数
	VECTOR2F mMousePosition;
	VECTOR3F mWorldNearPosition;
	VECTOR3F mWorldFarPosition;
	std::unique_ptr<FrameBuffer>mStageSidoView;
	std::unique_ptr<Sprite>mSprite;
	std::unique_ptr<Camera>mSidoCamera;
	VECTOR3F mStageLeftPosition;
	VECTOR3F mStageRightPosition;
	VECTOR2F mSidoViewRenderPosition;
	VECTOR2F mSidoViewRenderSize;
	bool mCreateFlag;
	int mWidth;
	int mHeight;
	int mDragObjNo;
	std::unique_ptr<Obj3D>mObj;
	StageData mCreateData;
	bool mEditorFlag;
	int mFileState;
	int mEditorState;
	int mDeleteNum;
	bool mSaveSceneFlag;
};