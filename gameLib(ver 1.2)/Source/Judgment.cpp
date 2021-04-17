#include "Judgment.h"
#include"collision.h"
#include"camera_manager.h"
#include"hit_area_render.h"
//プレイヤーとステージとの当たり判定関数

void Judgment::Judge(PlayerCharacter* player, StageManager* manager)
{
	//プレイヤーのパラメーターを取得
	VECTOR3F playerPosition = player->GetPosition();
	VECTOR3F playerBefore = player->GetBeforePosition();
	VECTOR3F playerScale = player->GetScale();
	//プレイヤーの当たり判定のサイズをもとに座標の最大値と最小値を決める
	static const VECTOR3F hitSize = VECTOR3F(1.15f, 6.6f, 1.3f);
	VECTOR3F playerMin = playerPosition - VECTOR3F(hitSize.x, 0, hitSize.z) * playerScale;
	VECTOR3F playerMax = playerPosition + hitSize * playerScale;
	//動いた分だけサイズを大きくする
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
	//当たり判定のサイズを当たり判定描画用クラスに送る
	VECTOR3F size = playerMax - playerMin;
	HitAreaRender::GetInctance()->SetObjData(playerMin + size*0.5f, size * 0.5f);
	bool groundFlaf = false;

	for (auto& stage : manager->GetStages())
	{
		//ステージオブジェクトの不透明度が1かどうかを調べる(違ったらcontinue)
		if (stage->GetColor().w < 1)continue;
		//ステージの当たり判定サイズを計算する
		VECTOR3F stageMin = stage->GetPosition() - stage->GetScale();
		VECTOR3F stageMax = stage->GetPosition() + stage->GetScale();
		switch (stage->GetStageData().mObjType)
		{
		case 0://床や壁
			if (Collision::IsHitAABB(playerMin, playerMax, stageMin, stageMax, nullptr))
			{
				if (playerBefore.z + hitSize.z * playerScale.z <= stageMin.z && playerPosition.z + hitSize.z * playerScale.z >= stageMin.z)
				{//床の前
					//プレイヤーの座標修正
					playerPosition = VECTOR3F(playerPosition.x, playerPosition.y, stageMin.z - hitSize.z * playerScale.z);
					playerMax.z = stageMin.z;
				}
				else if (playerBefore.y >= stageMax.y && playerPosition.y <= stageMax.y)
				{//床の上
					groundFlaf = true;
					if (stageMax.z - playerPosition.z <= 2.5f* gameObjScale)
					{//床の奥の方だったらジャンプする
						player->SetMoveState(PlayerCharacter::MOVESTATE::JUMP);
					}
					else
					{//そうでなかったら通常移動
						player->SetMoveState(PlayerCharacter::MOVESTATE::MOVE);
					}
					//プレイヤーの座標修正
					playerPosition = VECTOR3F(playerPosition.x, stageMax.y, playerPosition.z);
					playerMin.y = stageMax.y;
				}
				else if (playerBefore.y + hitSize.y * playerScale.y < stageMin.y && playerPosition.y + hitSize.y * playerScale.y >= stageMin.y)
				{//床の下
					//プレイヤーの座標修正
					playerPosition = VECTOR3F(playerPosition.x, stageMin.y - hitSize.y * playerScale.y, playerPosition.z);
					player->SetVelocity(player->GetVelocity() * VECTOR3F(1, 0, 1));
					playerMax.y = stageMin.y;
				}

			}
			break;
		case 1://ジャンプ台
			//ステージの当たり判定サイズを計算する
			stageMin.y += stage->GetScale().y;
			stageMax.y += stage->GetScale().y;
			//プレイヤーの状態が通常移動中かどうか
			if (player->GetMoveState() == PlayerCharacter::MOVESTATE::RAMP)continue;
			if (player->GetMoveState() == PlayerCharacter::MOVESTATE::LANDING)continue;
			if (Collision::IsHitAABB(playerMin, playerMax, stageMin, stageMax, nullptr))
			{//条件が揃ったらジャンプ
				player->SetMoveState(PlayerCharacter::MOVESTATE::RAMP);
			}
			break;
		case 2://ゴール
			//ステージの当たり判定サイズを計算する
			stageMin = stage->GetPosition() - stage->GetScale() * VECTOR3F(1.5f, 3, 0.5f);
			stageMax = stage->GetPosition() + stage->GetScale() * VECTOR3F(1.5f, 3, 0.5f);
			if (Collision::IsHitAABB(playerMin, playerMax, stageMin, stageMax, nullptr))
			{//ゴール判定にする
				player->SetGorlFlag(true);
			}
			break;
		}
	}
	//プレイヤーのパラメーターをセット
	player->SetPosition(playerPosition);
	player->SetGroundFlag(groundFlaf);
	//空中にいたら落下中にする
	if (player->GetMoveState()==PlayerCharacter::MOVESTATE::MOVE && !groundFlaf)
	{
		player->SetMoveState(PlayerCharacter::MOVESTATE::LANDING);
	}
}
