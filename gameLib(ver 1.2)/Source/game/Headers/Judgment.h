#pragma once
#include"player_character.h"
#include"stage_manager.h"
//当たり判定クラス
class Judgment
{
public:
	//プレイヤーとステージとの当たり判定
	static void Judge(PlayerCharacter* player, StageManager* manager);
};