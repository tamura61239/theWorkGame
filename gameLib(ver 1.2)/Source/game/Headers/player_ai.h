#pragma once
#include"player_character.h"
#include"stage_manager.h"
#include"stage_operation.h"

class PlayerAI
{
public:
	//�R���X�g���N�^
	PlayerAI(PlayerCharacter* character);
	//�G�f�B�^�[
	void Editor(PlayerCharacter* character);
	//�X�V
	void Update(float elapsd_time,PlayerCharacter*character);
	//���X�|��
	void Reset();
private:
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
};