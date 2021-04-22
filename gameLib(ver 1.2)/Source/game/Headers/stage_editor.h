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
	int Editor(std::vector<std::shared_ptr<StageObj>>objs);
	//�~�j�}�b�v�̕`��
	void SidoViewRender(ID3D11DeviceContext* context);
	//��������I�u�W�F�N�g�̃p�����[�^�[�ݒ蒆�̕`��
	void EditorCreateObjImageRender(ID3D11DeviceContext* context, StaticMesh* mesh,MeshRender*render,VECTOR4F color);
private:
	//�}�E�X�X�V�֐�
	void MousePosition();
	//�X�N���[�����W�����[���h���W
	void ScreeenToWorld(VECTOR3F* worldPosition, const VECTOR3F& screenPosition, Camera* camera);
	//�}�E�X�������Ă郏�[���h��Ԃ̍��W�v�Z����
	void MouseToWorld(Camera*camera,const VECTOR2F&mousePosition);
	//sido�J�����֐�
	void SetSidoCamera(std::vector<std::shared_ptr<StageObj>>objs);
	//�}�E�X���N���b�N���Ă���I�u�W�F�N�g�𒲂ׂ�
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
	//�}�E�X�֘A�ϐ�
	VECTOR2F mMousePosition;
	VECTOR3F mWorldNearPosition;
	VECTOR3F mWorldFarPosition;
	//�����猩�����̑S�̂̉摜
	std::unique_ptr<FrameBuffer>mStageSidoView;
	std::unique_ptr<Sprite>mSprite;
	std::unique_ptr<Camera>mSidoCamera;
	//�����猩�����̉摜�̕`��ʒu
	VECTOR2F mSidoViewRenderPosition;
	VECTOR2F mSidoViewRenderSize;
	bool mCreateFlag;
	//�X�N���[���̃T�C�Y
	int mWidth;
	int mHeight;
	//�N���b�N�����I�u�W�F�N�g�̔z��ԍ�
	int mDragObjNo;
	//�������Ɏg���f�[�^
	std::unique_ptr<Obj3D>mObj;
	StageData mCreateData;

	bool mEditorFlag;
	//�t�@�C���̂ǂ̑�������邩
	int mFileState;
	//�G�f�B�^�̏��
	int mEditorState;
	//�����I�u�W�F�N�g�̔z��ԍ�
	int mDeleteNum;
	//�X�N���[���V���b�g����邩�ǂ���
	bool mSaveSceneFlag;
};