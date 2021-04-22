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
	int Editor(std::vector<std::shared_ptr<StageObj>>objs);
	//ミニマップの描画
	void SidoViewRender(ID3D11DeviceContext* context);
	//生成するオブジェクトのパラメーター設定中の描画
	void EditorCreateObjImageRender(ID3D11DeviceContext* context, StaticMesh* mesh,MeshRender*render,VECTOR4F color);
private:
	//マウス更新関数
	void MousePosition();
	//スクリーン座標→ワールド座標
	void ScreeenToWorld(VECTOR3F* worldPosition, const VECTOR3F& screenPosition, Camera* camera);
	//マウスがさしてるワールド空間の座標計算する
	void MouseToWorld(Camera*camera,const VECTOR2F&mousePosition);
	//sidoカメラ関数
	void SetSidoCamera(std::vector<std::shared_ptr<StageObj>>objs);
	//マウスがクリックしているオブジェクトを調べる
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
	//マウス関連変数
	VECTOR2F mMousePosition;
	VECTOR3F mWorldNearPosition;
	VECTOR3F mWorldFarPosition;
	//横から見た時の全体の画像
	std::unique_ptr<FrameBuffer>mStageSidoView;
	std::unique_ptr<Sprite>mSprite;
	std::unique_ptr<Camera>mSidoCamera;
	//横から見た時の画像の描画位置
	VECTOR2F mSidoViewRenderPosition;
	VECTOR2F mSidoViewRenderSize;
	bool mCreateFlag;
	//スクリーンのサイズ
	int mWidth;
	int mHeight;
	//クリックしたオブジェクトの配列番号
	int mDragObjNo;
	//生成時に使うデータ
	std::unique_ptr<Obj3D>mObj;
	StageData mCreateData;

	bool mEditorFlag;
	//ファイルのどの操作をするか
	int mFileState;
	//エディタの状態
	int mEditorState;
	//消すオブジェクトの配列番号
	int mDeleteNum;
	//スクリーンショットを取るかどうか
	bool mSaveSceneFlag;
};