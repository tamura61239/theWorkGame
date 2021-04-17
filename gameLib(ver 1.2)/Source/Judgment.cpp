#include "Judgment.h"
#include"collision.h"
#include"camera_manager.h"
#include"hit_area_render.h"
//�v���C���[�ƃX�e�[�W�Ƃ̓����蔻��֐�

void Judgment::Judge(PlayerCharacter* player, StageManager* manager)
{
	//�v���C���[�̃p�����[�^�[���擾
	VECTOR3F playerPosition = player->GetPosition();
	VECTOR3F playerBefore = player->GetBeforePosition();
	VECTOR3F playerScale = player->GetScale();
	//�v���C���[�̓����蔻��̃T�C�Y�����Ƃɍ��W�̍ő�l�ƍŏ��l�����߂�
	static const VECTOR3F hitSize = VECTOR3F(1.15f, 6.6f, 1.3f);
	VECTOR3F playerMin = playerPosition - VECTOR3F(hitSize.x, 0, hitSize.z) * playerScale;
	VECTOR3F playerMax = playerPosition + hitSize * playerScale;
	//�������������T�C�Y��傫������
	if (playerBefore.z > playerPosition.z)
	{
		playerMax.z = playerBefore.z + hitSize.z * playerScale.z;
	}
	else
	{
		playerMin.z = playerBefore.z - hitSize.z * playerScale.z;
	}
	if (playerBefore.y > playerPosition.y)
	{
		playerMax.y = playerBefore.y + hitSize.y * playerScale.y;
	}
	else
	{
		playerMin.y = playerBefore.y;
	}
	//�����蔻��̃T�C�Y�𓖂��蔻��`��p�N���X�ɑ���
	VECTOR3F size = playerMax - playerMin;
	HitAreaRender::GetInctance()->SetObjData(playerMin + size*0.5f, size * 0.5f);
	bool groundFlaf = false;

	for (auto& stage : manager->GetStages())
	{
		//�X�e�[�W�I�u�W�F�N�g�̕s�����x��1���ǂ����𒲂ׂ�(�������continue)
		if (stage->GetColor().w < 1)continue;
		//�X�e�[�W�̓����蔻��T�C�Y���v�Z����
		VECTOR3F stageMin = stage->GetPosition() - stage->GetScale();
		VECTOR3F stageMax = stage->GetPosition() + stage->GetScale();
		switch (stage->GetStageData().mObjType)
		{
		case 0://�����
			if (Collision::IsHitAABB(playerMin, playerMax, stageMin, stageMax, nullptr))
			{
				if (playerBefore.z + hitSize.z * playerScale.z <= stageMin.z && playerPosition.z + hitSize.z * playerScale.z >= stageMin.z)
				{//���̑O
					//�v���C���[�̍��W�C��
					playerPosition = VECTOR3F(playerPosition.x, playerPosition.y, stageMin.z - hitSize.z * playerScale.z);
					playerMax.z = stageMin.z;
				}
				else if (playerBefore.y >= stageMax.y && playerPosition.y <= stageMax.y)
				{//���̏�
					groundFlaf = true;
					if (stageMax.z - playerPosition.z <= 2.5f* gameObjScale)
					{//���̉��̕���������W�����v����
						player->SetMoveState(PlayerCharacter::MOVESTATE::JUMP);
					}
					else
					{//�����łȂ�������ʏ�ړ�
						player->SetMoveState(PlayerCharacter::MOVESTATE::MOVE);
					}
					//�v���C���[�̍��W�C��
					playerPosition = VECTOR3F(playerPosition.x, stageMax.y, playerPosition.z);
					playerMin.y = stageMax.y;
				}
				else if (playerBefore.y + hitSize.y * playerScale.y < stageMin.y && playerPosition.y + hitSize.y * playerScale.y >= stageMin.y)
				{//���̉�
					//�v���C���[�̍��W�C��
					playerPosition = VECTOR3F(playerPosition.x, stageMin.y - hitSize.y * playerScale.y, playerPosition.z);
					player->SetVelocity(player->GetVelocity() * VECTOR3F(1, 0, 1));
					playerMax.y = stageMin.y;
				}

			}
			break;
		case 1://�W�����v��
			//�X�e�[�W�̓����蔻��T�C�Y���v�Z����
			stageMin.y += stage->GetScale().y;
			stageMax.y += stage->GetScale().y;
			//�v���C���[�̏�Ԃ��ʏ�ړ������ǂ���
			if (player->GetMoveState() == PlayerCharacter::MOVESTATE::RAMP)continue;
			if (player->GetMoveState() == PlayerCharacter::MOVESTATE::LANDING)continue;
			if (Collision::IsHitAABB(playerMin, playerMax, stageMin, stageMax, nullptr))
			{//��������������W�����v
				player->SetMoveState(PlayerCharacter::MOVESTATE::RAMP);
			}
			break;
		case 2://�S�[��
			//�X�e�[�W�̓����蔻��T�C�Y���v�Z����
			stageMin = stage->GetPosition() - stage->GetScale() * VECTOR3F(1.5f, 3, 0.5f);
			stageMax = stage->GetPosition() + stage->GetScale() * VECTOR3F(1.5f, 3, 0.5f);
			if (Collision::IsHitAABB(playerMin, playerMax, stageMin, stageMax, nullptr))
			{//�S�[������ɂ���
				player->SetGorlFlag(true);
			}
			break;
		}
	}
	//�v���C���[�̃p�����[�^�[���Z�b�g
	player->SetPosition(playerPosition);
	player->SetGroundFlag(groundFlaf);
	//�󒆂ɂ����痎�����ɂ���
	if (player->GetMoveState()==PlayerCharacter::MOVESTATE::MOVE && !groundFlaf)
	{
		player->SetMoveState(PlayerCharacter::MOVESTATE::LANDING);
	}
}
