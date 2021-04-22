#pragma once
#include"stage_obj.h"
#include<vector>
#include"stage_editor.h"

class StageManager
{
public:
	//�R���X�g���N�^
	StageManager(ID3D11Device* device, int width, int height);
	//�X�e�[�W���S���ł������邩�𒲂ׂ�
	static void StageCount();
	//�X�e�[�W�̃I�u�W�F�N�g�̐���
	void Clear() { mStageObjs.clear(); }
	//�t�@�C������
	void Load();
	void Save();
	//�G�f�B�^
	void Editor();
	//�X�V
	void Update(float elapsd_time);
	//�`��
	void Render(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection, const int stageState,DrowShader*srv=nullptr);
	//���x�}�b�v�p
	void RenderVelocity(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection, const int stageState);
	//�e�p
	void RenderShadow(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection);
	//�����猩�����̃V�[��
	void SidoViewRender(ID3D11DeviceContext* context);
	//setter
	void SetStageNo(int no) { mStageNo = no; }
	//getter
	std::vector<std::shared_ptr<StageObj>>GetStages() { return mStageObjs; }
	StageEditor* GetStageEditor() { return mEditor.get(); }
	static const int GetMaxStageCount() { return mMaxStage; }
	const int GrtStageNo() { return mStageNo; }
private:
	//�`��p�ϐ�
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbBeforeBuffer;
	std::unique_ptr<DrowShader>mVelocityShader;
	std::unique_ptr<DrowShader>mDeferredShader;
	std::unique_ptr<MeshRender>mRender;
	//�X�e�[�W�̍ő吔
	static int mMaxStage;
	//�X�e�[�W�̃I�u�W�F�N�g
	std::vector < std::shared_ptr<StageObj>>mStageObjs;
	//�G�f�B�^
	std::unique_ptr<StageEditor>mEditor;
	//�X�e�[�W�ԍ�
	int mStageNo;
	//�X�e�[�W�̃I�u�W�F�N�g�̎��
	std::vector<std::shared_ptr<StaticMesh>>mMeshs;
	//editor�ϐ�
	float mWidth;
	float mHeight;
	VECTOR3F mNearMouse;
	VECTOR3F mFarMouse;
	int dragObjNumber;
};