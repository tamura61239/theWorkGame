#pragma once
#include"scene.h"
#include<memory>
#include"sprite.h"
#include"bloom.h"
#include"blend_state.h"
#include"ranking.h"
#include"fade.h"
#include"sky_map.h"
#include"stage_manager.h"
#include"render_state.h"
#include"sampler_state.h"


class SceneResult :public Scene
{
public:
	//�R���X�g���N�^
	SceneResult(float timer,int nowStageNo);
	//������
	void Initialize(ID3D11Device* device);
	//�G�f�B�^�[
	void Editor();
	//�X�V
	void Update(float elapsed_time);
	//�`��
	void Render(ID3D11DeviceContext* context, float elapsed_time);
	//���
	void Relese();
private:
	//�摜�`��p�ϐ�
	std::unique_ptr<Sprite>mNumberText;
	std::unique_ptr<Sprite>mRenderScene;
	//���[�ނ̃N���A�^�C��
	float mNowGameTime;
	//�i�s�\�t���O
	bool mPlayFlag;
	//NowLoading���Ɏg���ϐ�
	bool nowLoading;
	bool renderFlag;
	std::unique_ptr<Ranking>mRanking;
	//�`��p�ϐ�
	std::shared_ptr<FrameBuffer>frameBuffer;
	std::unique_ptr<FrameBuffer>frameBuffer2;
	std::shared_ptr<FrameBuffer>velocityBuffer;
	std::unique_ptr<BloomRender>mBloom;
	std::unique_ptr<Fade>mFade;
	std::unique_ptr<SkyMap>sky;
	//samplerState�̃^�C�v
	enum samplerType
	{
		wrap, clamp, max
	};
};
