#include "Judgment.h"
#include"collision.h"

void Judgment::Judge(PlayerCharacter* player, StageManager* manager)
{
	VECTOR3F playerPosition = player->GetPosition();
	VECTOR3F playerBefore = player->GetBeforePosition();
	VECTOR3F playerScale = player->GetScale();
	VECTOR3F playerMin = playerPosition - VECTOR3F(1.15f, 0, 1.15f) * playerScale;
	VECTOR3F playerMax = playerPosition + VECTOR3F(1.15f, 6.6f, 1.15f) * playerScale;
	for (auto& stage : manager->GetStages())
	//auto stage = manager->GetStages()[0];
	{
		VECTOR3F stageMin = stage->GetPosition() - stage->GetScale();
		VECTOR3F stageMax = stage->GetPosition() + stage->GetScale();
		if (Collision::IsHitAABB(playerMin, playerMax, stageMin, stageMax, nullptr))
		{
			switch (stage->GetStageData().mObjType)
			{
			case 0:
				if (playerBefore.z + 1.15 * playerScale.z <= stageMin.z && playerPosition.z + 1.15f * playerScale.z >= stageMin.z)//°‚Ì‘O
				{
					player->SetPosition(VECTOR3F(playerPosition.x, playerPosition.y, stageMin.z - 1.15f * playerScale.z));
				}
				else if (playerBefore.y >= stageMax.y && playerPosition.y <= stageMax.y)//°‚Ìã
				{
					if (stageMax.z - playerPosition.z <= stage->GetScale().z * 0.04f)
					{
						player->SetMoveState(PlayerCharacter::MOVESTATE::JUMP);
					}
					else
					{
						player->SetMoveState(PlayerCharacter::MOVESTATE::MOVE);
					}
					player->SetPosition(VECTOR3F(playerPosition.x, stageMax.y, playerPosition.z));
				}
				else if (playerBefore.y + 6.6f * playerScale.y < stageMin.y && playerPosition.y + 6.6f * playerScale.y >= stageMin.y)//°‚Ì‰º
				{
					player->SetPosition(VECTOR3F(playerPosition.x, stageMin.y - 6.6f * playerScale.y, playerPosition.z));
					player->SetVelocity(player->GetVelocity() * VECTOR3F(1, 0, 1));
				}
				playerPosition = player->GetPosition();
				break;
			}
			//if (stageMax.y > playerMin.y)
			//{
			//	player->SetPosition(VECTOR3F(player->GetPosition().x, stage->GetPosition().y + stage->GetScale().y, player->GetPosition().z));
			//	player->SetMoveState(PlayerCharacter::MOVESTATE::MOVE);
			//}
		}
		else
		{
			if (!player->GetChangState())
			{
				player->SetMoveState(PlayerCharacter::MOVESTATE::LANDING);
			}
		}
	}
	player->CalculateBoonTransform(0);
}
