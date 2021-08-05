#pragma once
#include"tutorial_state.h"
#include"fade.h"
#include"player_manager.h"
#include"zoom_blur_parameter.h"

class SceneGameMove
{
public:
	//�R���X�g���N�^
	SceneGameMove(ID3D11Device* device, int stageNo);
	//�Y�[���u���[�̃G�f�B�^
	void ZoomBlurEditor();
	//�X�V
	float Update(float elapsdTime, PlayerManager* player,bool testGame,bool editorFlag);
	//�`��
	void Render(ID3D11DeviceContext* context, ID3D11ShaderResourceView* srv,Sprite*render);
	//getter
	TutorialState* GetTutorial() { return mTutorial.get(); }
	ConstantBuffer<CbZoom>* GetCbZoomBuffer() { return mCbZoomBlurBuffer.get(); }
private:
	//�V�[���̐i�s�֐�
	float SceneEnd(float elapsdTime);
	float SceneStart(float elapsdTime, bool editorFlag);
	float SceneMove(float elapsdTime, PlayerManager* player, bool testGame);
	//�ϐ�
	std::unique_ptr<TutorialState>mTutorial;
	std::unique_ptr<Fade>mFade;
	std::unique_ptr<ConstantBuffer<CbZoom>>mCbZoomBlurBuffer;
	std::unique_ptr<DrowShader>mZoomBlurShader;
	bool mGorlFlag;
	int mStageNo;
};