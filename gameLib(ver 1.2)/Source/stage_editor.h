#pragma once
#include"stage_obj.h"
#include"framebuffer.h"
#include"sprite.h"
#include"camera.h"

class StageEditor
{
public:
	//�R���X�g���N�^
	StageEditor(ID3D11Device*device,int width,int height);
	//�X�e�[�W�f�[�^�̃��[�h
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
	//�X�V
	int Update(std::vector<std::shared_ptr<StageObj>>objs);
	//�`��
	void Render(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection);
	void SidoViewRender(ID3D11DeviceContext* context);
	void EditorCreateObjImageRender(ID3D11DeviceContext* context, StaticMesh* mesh,MeshRender*render,VECTOR4F color);
private:
	//�}�E�X�X�V�֐�
	void UpdateMouseData();
	/*********�}�E�X���W�ϊ��֐�**********/
	//�X�N���[�����W�����[���h���W
	void ScreeenToWorld(VECTOR3F* worldPosition, const VECTOR3F& screenPosition, Camera* camera);
	void MouseToWorld(Camera*camera,const VECTOR2F&mousePosition);
	//sido�J�����֐�
	void SetSidoCamera(std::vector<std::shared_ptr<StageObj>>objs);
	/*********�}�E�X���N���b�N���Ă���obj����********/
	void SearchStageObj(std::vector<std::shared_ptr<StageObj>>objs);
	int NewDragObj(std::vector<std::shared_ptr<StageObj>>objs);
	//�}�E�X�̈ʒu����
	bool MouseJudg();
	//�J�����̐V�������W�v�Z
	void NewCameraPosition();
	//�ݒ�
	void CreateDataSetTransformData()
	{
		static float s = gameObjScale / 10.0f;
		mCreateData.mAngle = mObj->GetAngle();
		mCreateData.mScale = mObj->GetScale() / s;
		mCreateData.mPosition = mObj->GetPosition() / s;
		mCreateFlag = true;
	}
	//�ϐ�
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