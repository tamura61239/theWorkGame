#include "Judgment.h"
#include"collision.h"
#include"camera_manager.h"
#include"hit_area_drow.h"

void Judgment::Judge(PlayerCharacter* player, StageManager* manager)
{
	VECTOR3F playerPosition = player->GetPosition();
	VECTOR3F playerBefore = player->GetBeforePosition();
	VECTOR3F playerScale = player->GetScale();
	static const VECTOR3F hitSize = VECTOR3F(1.15f, 6.6f, 1.3f);
	VECTOR3F playerMin = playerPosition - VECTOR3F(hitSize.x, 0, hitSize.z) * playerScale;
	VECTOR3F playerMax = playerPosition + hitSize * playerScale;
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
				if (playerBefore.z + hitSize.z * playerScale.z <= stageMin.z && playerPosition.z + hitSize.z * playerScale.z >= stageMin.z)//°‚Ì‘O
				{
					playerPosition = VECTOR3F(playerPosition.x, playerPosition.y, stageMin.z - hitSize.z * playerScale.z);

					playerMax.z = stageMin.z;
				}
				else if (playerBefore.y >= stageMax.y && playerPosition.y <= stageMax.y)//°‚Ìã
				{
					groundFlaf = true;
					if (stageMax.z - playerPosition.z <= 2.5f* gameObjScale)
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
				else if (playerBefore.y + hitSize.y * playerScale.y < stageMin.y && playerPosition.y + hitSize.y * playerScale.y >= stageMin.y)//°‚Ì‰º
				{
					playerPosition = VECTOR3F(playerPosition.x, stageMin.y - hitSize.y * playerScale.y, playerPosition.z);
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
		case 2:
			stageMin = stage->GetPosition() - stage->GetScale() * VECTOR3F(1.5f, 3, 0.5f);
			stageMax = stage->GetPosition() + stage->GetScale() * VECTOR3F(1.5f, 3, 0.5f);
			if (Collision::IsHitAABB(playerMin, playerMax, stageMin, stageMax, nullptr))
			{
				player->SetGorlFlag(true);
			}
			break;
		}
	}
	pHitAreaDrow.SetCube(playerMin, playerMax);
	player->SetPosition(playerPosition);
	player->SetGroundFlag(groundFlaf);
	if (player->GetMoveState()==PlayerCharacter::MOVESTATE::MOVE && !groundFlaf)
	{
		player->SetMoveState(PlayerCharacter::MOVESTATE::LANDING);
	}
}
