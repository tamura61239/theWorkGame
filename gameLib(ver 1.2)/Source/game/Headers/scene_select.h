#pragma once
#include"scene.h"
#include"stage_select.h"
#include"bloom.h"

//�Z���N�g�V�[��
class SceneSelect :public Scene
{
public:
	//�R���X�g���N�^
	SceneSelect(){}
	//������
	void Initialize(ID3D11Device* device);
	//ImGui�Ńp�����[�^�[�𒲐������肷��p�̊֐�
	void Editor();
	//�X�V
	void Update(float elapsed_time);
	//�`��
	void Render(ID3D11DeviceContext* context, float elapsed_time);
	//���
	void Relese();

private:
	//�`��p�ϐ�
	std::unique_ptr<Sprite>mRenderTexture;
	std::unique_ptr<FrameBuffer>mColorMap;
	std::unique_ptr<FrameBuffer>mSceneFrame;
	std::unique_ptr<BloomRender>mBloom;
	//�X�e�[�W��I�Ԃ̂Ɏg���ϐ�
	std::unique_ptr<StageSelect>mSelect;
};