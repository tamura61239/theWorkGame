#pragma once
#include"player_character.h"
#include"player_ai.h"
#include"model_renderer.h"
#include"stage_manager.h"

class PlayerManager
{
public:
	//�R���X�g���N�^
	PlayerManager(ID3D11Device* device, const char* fileID);
	//�G�f�B�^�֐�
	void Editor();
	//�X�V
	void Update(float elapsdTime,StageManager*stageManager);
	//���X�|��
	void Respond();
	//�`��
	void Render(ID3D11DeviceContext* context,int type);
	enum RENDER_TYPE
	{
		COLOR,
		SHADOW,
		VELOCITY
	};
	//getter
	PlayerCharacter* GetCharacter() { return mCharacter.get(); }
	bool GetPlayFlag() { return mPlayFlag; }
	//setter
	void SetPlayFlag(const bool playFlag) { mPlayFlag = playFlag; }
private:
	//���x�}�b�v�̕`��
	void RenderVelocity(ID3D11DeviceContext* context);
	//�V���h�E�}�b�v�̕`��
	void RenderShadow(ID3D11DeviceContext* context);
	//�ϐ�
	std::unique_ptr<PlayerCharacter>mCharacter;
	std::unique_ptr<PlayerAI>mPlayer;
	std::unique_ptr<ModelRenderer>mRender;
	bool mPlayFlag;
};