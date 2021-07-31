#pragma once
#include"player_character.h"
#include"stage_manager.h"
#include"stage_operation.h"

class PlayerAI
{
public:
	//�R���X�g���N�^
	PlayerAI(ID3D11Device* device, const char* fileName);
	//�G�f�B�^�[
	void Editor();
	//setter
	void SetPlayFlag(const bool flag) { mPlayFlag = flag; }
	//getter
	const bool GetPlayFlag() { return mPlayFlag; }
	PlayerCharacter* GetCharacter() { return mCharacter.get(); }
	//�X�V
	void Update(float elapsd_time,StageManager*manager,StageOperation*operation);
	//player�𓮂���
	void Move(float elapsd_time);
	//���X�|��
	void Respond();
private:
	//player�I�u�W�F�N�g�ϐ�
	std::unique_ptr<PlayerCharacter>mCharacter;
	//�G�f�B�^�[�ϐ�
	struct PlayerParameter
	{
		VECTOR3F accel;
		float maxSpeed;
		float minSpeed;
		VECTOR3F jump;
		VECTOR3F ranp;
		float gravity;
	};
	PlayerParameter mParameter;
	//�d��
	float mGravity;
	//�v���C�ł��邩�ǂ���
	bool mPlayFlag;
};