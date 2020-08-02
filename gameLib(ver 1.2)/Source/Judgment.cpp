#include "Judgment.h"
#include"collision.h"
#include"camera_manager.h"
#include"hit_area_drow.h"

void Judgment::Judge(PlayerCharacter* player, StageManager* manager)
{
	VECTOR3F playerPosition = player->GetPosition();
	VECTOR3F playerBefore = player->GetBeforePosition();
	VECTOR3F playerScale = player->GetScale();
	VECTOR3F playerMin = playerPosition - VECTOR3F(1.15f, 0, 1.15f) * playerScale;
	VECTOR3F playerMax = playerPosition + VECTOR3F(1.15f, 6.6f, 1.15f) * playerScale;
	if (playerBefore.z > playerPosition.z)
	{
		playerMax.z = playerBefore.z + 1.15f * playerScale.z;
	}
	else
	{
		playerMin.z = playerBefore.z - 1.15f * playerScale.z;
	}
	if (playerBefore.y > playerPosition.y)
	{
		playerMax.y = playerBefore.y + 6.6f * playerScale.y;
	}
	else
	{
		playerMin.y = playerBefore.y;
	}
	bool groundFlaf = false;
	for (auto& stage : manager->GetStages())
		//auto stage = manager->GetStages()[0];
	{
		if (stage->GetColor().w < 1)continue;
		VECTOR3F stageMin = stage->GetPosition() - stage->GetScale();
		VECTOR3F stageMax = stage->GetPosition() + stage->GetScale();
		//if (Collision::IsHitAABB(playerMin, playerMax, stageMin, stageMax, nullptr))
		//{
		switch (stage->GetStageData().mObjType)
		{
		case 0:
			if (Collision::IsHitAABB(playerMin, playerMax, stageMin, stageMax, nullptr))
			{
				if (playerBefore.z + 1.15 * playerScale.z <= stageMin.z && playerPosition.z + 1.15f * playerScale.z >= stageMin.z)//°‚Ì‘O
				{
					playerPosition = VECTOR3F(playerPosition.x, playerPosition.y, stageMin.z - 1.15f * playerScale.z);

					playerMax.z = stageMin.z;
				}
				else if (playerBefore.y >= stageMax.y && playerPosition.y <= stageMax.y)//°‚Ìã
				{
					groundFlaf = true;
					if (stageMax.z - playerPosition.z <= 25.0f)
					{
						player->SetMoveState(PlayerCharacter::MOVESTATE::JUMP);
					}
					else
					{
						player->SetMoveState(PlayerCharacter::MOVESTATE::MOVE);
					}
					playerPosition = VECTOR3F(playerPosition.x, stageMax.y, playerPosition.z);
					playerMin.y = stageMax.y;
				}
				else if (playerBefore.y + 6.6f * playerScale.y < stageMin.y && playerPosition.y + 6.6f * playerScale.y >= stageMin.y)//°‚Ì‰º
				{
					playerPosition = VECTOR3F(playerPosition.x, stageMin.y - 6.6f * playerScale.y, playerPosition.z);
					player->SetVelocity(player->GetVelocity() * VECTOR3F(1, 0, 1));
					playerMax.y = stageMin.y;
				}

			}
			break;
		case 1:
			stageMin.y += stage->GetScale().y;
			stageMax.y += stage->GetScale().y;
			if (player->GetMoveState() == PlayerCharacter::MOVESTATE::RAMP)continue;
			if (player->GetMoveState() == PlayerCharacter::MOVESTATE::LANDING)continue;
			if (Collision::IsHitAABB(playerMin, playerMax, stageMin, stageMax, nullptr))
			{
				//player->SetPosition(playerPosition + VECTOR3F(0, stageMin.y + 3.f, 0));
				//playerPosition = player->GetPosition();
				//player->SetVelocity(player->GetVelocity() * VECTOR3F(1, 1, 0.6f));

				player->SetMoveState(PlayerCharacter::MOVESTATE::RAMP);
			}

			break;
		}
	}
	pHitAreaDrow.SetCube(playerMin, playerMax);
	player->SetPosition(playerPosition);
	player->SetGroundFlag(groundFlaf);
	if (!player->GetChangState() && !groundFlaf)
	{
		player->SetMoveState(PlayerCharacter::MOVESTATE::LANDING);
	}
}
