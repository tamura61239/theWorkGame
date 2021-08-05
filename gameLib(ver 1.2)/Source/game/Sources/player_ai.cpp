#include "player_ai.h"
//#include"camera_manager.h"
//#include"Judgment.h"
//#include"hit_area_render.h"
#include"file_function.h"

#ifdef USE_IMGUI
#include<imgui.h>
#endif
/*****************************************************/
//　　　　　　　　　　初期化関数(コンストラクタ)
/*****************************************************/
PlayerAI::PlayerAI(PlayerCharacter* character) : mGravity(0)
{
	////playerオブジェクトの生成
	//std::unique_ptr<ModelData>data = std::make_unique<ModelData>(fileName);
	//std::shared_ptr<ModelResource>resouce = std::make_shared<ModelResource>(device, std::move(data));
	//mCharacter = std::make_unique<PlayerCharacter>(resouce);
	//初期座標の設定
	character->SetPosition(VECTOR3F(0, 10, 0));
	character->SetScale(VECTOR3F(gameObjScale, gameObjScale, gameObjScale));
	character->CalculateBoonTransform(0);
	//パラメーターの設定
	memset(&mParameter, 0, sizeof(mParameter));
	FileFunction::Load(mParameter, "Data/file/playerParrameter.bin", "rb");
	character->SetMinSpeed(mParameter.minSpeed);
	character->SetMaxSpeed(mParameter.maxSpeed);
}

/*****************************************************/
//　　　　　　　　　　エディタ関数
/*****************************************************/
void PlayerAI::Editor(PlayerCharacter* character)
{
#ifdef USE_IMGUI
	ImGui::Begin("player");
	//加速度
	float* accel[3] = { &mParameter.accel.x,&mParameter.accel.y ,&mParameter.accel.z };
	ImGui::InputFloat("accel", &mParameter.accel.z, 10);
	//ジャンプ力
	float* j[3] = { &mParameter.jump.x,&mParameter.jump.y,&mParameter.jump.z };
	ImGui::InputFloat("jump", &mParameter.jump.y, 10);
	//ジャンプ台で飛ぶ力
	float* r[3] = { &mParameter.ranp.x,&mParameter.ranp.y,&mParameter.ranp.z };
	ImGui::InputFloat("ranp", &mParameter.ranp.y, 10);
	//最大速度
	if (ImGui::InputFloat("maxSpeed", &mParameter.maxSpeed, 10))
	{
		character->SetMaxSpeed(mParameter.maxSpeed);
	}
	//最小速度
	if (ImGui::InputFloat("minSpeed", &mParameter.minSpeed, 10))
	{
		character->SetMinSpeed(mParameter.minSpeed);
	}
	//重力値
	ImGui::InputFloat("gravity", &mParameter.gravity, 10);
	//セーブ
	if (ImGui::Button("save"))
	{
		FileFunction::Save(mParameter, "Data/file/playerParrameter.bin", "wb");

	}
	//ロード
	if (ImGui::Button("load"))
	{
		FileFunction::Load(mParameter, "Data/file/playerParrameter.bin", "rb");

	}
	//パラメーターの表示
	ImGui::Text("position:%f,%f,%f", character->GetPosition().x, character->GetPosition().y, character->GetPosition().z);
	ImGui::Text("velocity:%f,%f,%f", character->GetVelocity().x, character->GetVelocity().y, character->GetVelocity().z);
	ImGui::Text("state:%d", character->GetMoveState());
	ImGui::Text("changeFlg:%d", character->GetChangState());
	ImGui::End();
#endif
}

/*****************************************************/
//　　　　　　　　　　更新関数
/*****************************************************/
/********************playerの更新******************/

void PlayerAI::Update(float elapsd_time, PlayerCharacter*character)
{
	//速度、加速度の取得
	VECTOR3F accel = character->GetAccel();
	VECTOR3F velocity = character->GetVelocity();
	//状態の取得
	auto state = character->GetMoveState();
	//加速度を設定
	accel.z = mParameter.accel.z;
	//重力値を更新
	mGravity += mParameter.gravity * elapsd_time;
	//加速度に重力値を足す
	accel.y += mGravity * 60 * elapsd_time;
	//空中にいないかどうかを判定する
	if (character->GetGroundFlag())
	{
		mGravity = mParameter.gravity;
		velocity.y = 0;
		accel.y = 0;
	}
	//状態に合わせて速度を変化させる
	if (state == PlayerCharacter::MOVESTATE::JUMP)
	{
		velocity.y = mParameter.jump.y;
		state = PlayerCharacter::MOVESTATE::LANDING;
	}
	if (state == PlayerCharacter::MOVESTATE::RAMP)
	{
		velocity.y = mParameter.ranp.y;
		state = PlayerCharacter::MOVESTATE::LANDING;
	}
	//ゴールした時のキャラクターの動き
	if (character->GetGorlFlag())
	{
		VECTOR3F angle = character->GetAngle();
		if (angle.y < DirectX::XMConvertToRadians(135))
		{
			angle.y += 3.14f * elapsd_time * 1.05f;
			character->SetAngle(angle);
		}
		accel.z = -velocity.z * 3.f;
		accel.y = 0;
		velocity.y = 0;
	}
	//更新したデータのセット
	character->SetAccel(accel);
	character->SetVelocity(velocity);
	character->SetMoveState(state);

}

void PlayerAI::Reset()
{
	mGravity = mParameter.gravity;

}
