#pragma once
#include"scene.h"
#include<memory>
#include"sprite.h"
#include"static_obj.h"
#include"framebuffer.h"
#include"render_effects.h"
#include"model.h"
#include"model_renderer.h"
#include"bloom.h"
#include"player_ai.h"
#include"stage_manager.h"
#include"stage_operation.h"
#include"sky_map.h"
#include"drow_shader.h"
#include"stage_select.h"
#include"light_view.h"
#include"tutorial_state.h"
#include"zoom_blur_parameter.h"
#include"constant_buffer.h"
#include"sampler_state.h"
#include"render_state.h"
#include"motion_blur_parameter.h"


class SceneGame :public Scene
{
public:
	SceneGame(int stageNo);
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
	//�t�F�[�h���̓���
	void FadeMove(float elapsdTime);
	//�摜�`��ϐ�
	std::unique_ptr<Sprite>test;
	std::unique_ptr<Sprite>nowLoading;
	std::unique_ptr<Sprite>siro;
	std::unique_ptr<Sprite>pushKey;
	//�f�v�X�o�b�t�@
	std::unique_ptr<FrameBuffer>shadowMap;
	std::shared_ptr<FrameBuffer>frameBuffer;
	std::shared_ptr<FrameBuffer>frameBuffer3;
	std::shared_ptr<FrameBuffer>velocityMap;
	std::shared_ptr<FrameBuffer>shadowRenderBuffer;
	std::shared_ptr<FrameBuffer>frameBuffer2;
	//�Q�[���𓮂����̂ɕK�v�ȕϐ�
	std::shared_ptr<PlayerAI>player;
	std::unique_ptr<StageManager>mSManager;
	std::unique_ptr<StageOperation>mStageOperation;
	std::unique_ptr<TutorialState>mTutorialState;
	//�`��ɕK�v�ȕϐ�
	std::unique_ptr<RenderEffects>mRenderEffects;
	std::unique_ptr<ModelRenderer>mModelRenderer;
	std::unique_ptr<BloomRender>mBloom;
	std::unique_ptr<SkyMap>mSky;
	std::unique_ptr<DrowShader>mBlurShader;
	std::unique_ptr<DrowShader>motionBlurShader;
	std::unique_ptr<DrowShader>skymapMotionShader;
	std::unique_ptr<LightView>mLightView;
	std::unique_ptr<ConstantBuffer<CbZoom>>mCbZoomBuffer;
	std::unique_ptr<ConstantBuffer<MotionBlurParameter>>mCbMotionBlur;
	//samplerState�̃^�C�v
	enum samplerType
	{
		warp,border, clamp,max
	};
	//�G�f�B�^�[�ϐ�
	bool testGame;
	bool hitArea;
	int textureNo;
	//NowLoading�̎��Ɏg���ϐ�
	bool mNowLoading;
	bool mLoadEnd;
	//�X�e�[�W�ԍ��ێ��ϐ�
	int mStageNo;
};