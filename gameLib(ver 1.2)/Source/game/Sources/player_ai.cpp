#include "player_ai.h"
#include"camera_manager.h"
#include"Judgment.h"
#include"hit_area_render.h"
#include"file_function.h"

#ifdef USE_IMGUI
#include<imgui.h>
#endif
/*****************************************************/
//　　　　　　　　　　初期化関数(コンストラクタ)
/*****************************************************/
PlayerAI::PlayerAI(ID3D11Device* device, const char* fileName) :mPlayFlag(false), mGravity(0)
{
	//playerオブジェクトの生成
	std::unique_ptr<ModelData>data = std::make_unique<ModelData>(fileName);
	std::shared_ptr<ModelResource>resouce = std::make_shared<ModelResource>(device, std::move(data));
	mCharacter = std::make_unique<PlayerCharacter>(resouce);
	//初期座標の設定
	mCharacter->SetPosition(VECTOR3F(0, 10, 0));
	mCharacter->SetScale(VECTOR3F(gameObjScale, gameObjScale, gameObjScale));
	mCharacter->CalculateBoonTransform(0);
	//パラメーターの設定
	memset(&mParameter, 0, sizeof(mParameter));
	FileFunction::Load(mParameter, "Data/file/playerParrameter.bin", "rb");
	mCharacter->SetMinSpeed(mParameter.minSpeed);
	mCharacter->SetMaxSpeed(mParameter.maxSpeed);
}

/*****************************************************/
//　　　　　　　　　　エディタ関数
/*****************************************************/
void PlayerAI::Editor()
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
		mCharacter->SetMaxSpeed(mParameter.maxSpeed);
	}
	//最小速度
	if (ImGui::InputFloat("minSpeed", &mParameter.minSpeed, 10))
	{
		mCharacter->SetMinSpeed(mParameter.minSpeed);
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
	//player座標などのリセット
	if (ImGui::Button("replay"))
	{
		mCharacter->SetPosition(VECTOR3F(0, 10, 0));
		mCharacter->SetBeforePosition(VECTOR3F(0, 10, 0));
		mCharacter->SetVelocity(VECTOR3F(0, 0, 0));
		mCharacter->SetAngle(VECTOR3F(0, 0, 0));
		mCharacter->CalculateBoonTransform(0);
		mCharacter->SetMoveState(PlayerCharacter::MOVESTATE::LANDING);

	}
	//パラメーターの表示
	ImGui::Text("position:%f,%f,%f", mCharacter->GetPosition().x, mCharacter->GetPosition().y, mCharacter->GetPosition().z);
	ImGui::Text("velocity:%f,%f,%f", mCharacter->GetVelocity().x, mCharacter->GetVelocity().y, mCharacter->GetVelocity().z);
	ImGui::Text("state:%d", mCharacter->GetMoveState());
	ImGui::Text("changeFlg:%d", mCharacter->GetChangState());
	ImGui::End();
#endif
}

/*****************************************************/
//　　　　　　　　　　更新関数
/*****************************************************/
/********************playerの更新******************/

void PlayerAI::Update(float elapsd_time, StageManager* manager, StageOperation* operation)
{
	if (mPlayFlag)
	{	//プレイ中

		//一定以上下に落ちたらスタート時点に戻る
		if (mCharacter->GetPosition().y < -1000)
		{
			if (mCharacter->GetExist())
			{
				mCharacter->SetExist(false);
				operation->Reset(manager);

			}
			return;
		}
		//動けるとき
		Move(elapsd_time);
	}
	else
	{//プレイしてない時
		elapsd_time = 0;
		mCharacter->SetGorlFlag(false);
		mCharacter->SetGroundFlag(false);
		mGravity = mParameter.gravity;
	}
	//ステージとの当たり判定
	Judgment::Judge(mCharacter.get(), manager);
	//カメラにplayerの位置を設定
	pCameraManager->GetCameraOperation()->GetPlayCamera()->SetPlayerPosition(mCharacter->GetPosition());
	//アニメーションの更新
	mCharacter->AnimUpdate(elapsd_time);
}
/********************playerを動かす******************/
void PlayerAI::Move(float elapsd_time)
{
	//速度、加速度の取得
	VECTOR3F accel = mCharacter->GetAccel();
	VECTOR3F velocity = mCharacter->GetVelocity();
	//状態の取得
	auto state = mCharacter->GetMoveState();
	//加速度を設定
	accel.z = mParameter.accel.z;
	//重力値を更新
	mGravity += mParameter.gravity * elapsd_time;
	//加速度に重力値を足す
	accel.y += mGravity * 60 * elapsd_time;
	//空中にいないかどうかを判定する
	if (mCharacter->GetGroundFlag())
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
	if (mCharacter->GetGorlFlag())
	{
		VECTOR3F angle = mCharacter->GetAngle();
		if (angle.y < DirectX::XMConvertToRadians(135))
		{
			angle.y += 3.14f * elapsd_time * 1.05f;
			mCharacter->SetAngle(angle);
		}
		accel.z = -velocity.z * 3.f;
		accel.y = 0;
		velocity.y = 0;
	}
	//更新したデータのセット
	mCharacter->SetAccel(accel);
	mCharacter->SetVelocity(velocity);
	mCharacter->SetMoveState(state);
	//座標などの更新
	mCharacter->Move(elapsd_time);

}

void PlayerAI::Respond()
{
	auto camera = pCameraManager->GetCameraOperation()->GetPlayCamera();

	mCharacter->SetPosition(VECTOR3F(0, 10, 0));
	mCharacter->SetBeforePosition(VECTOR3F(0, 10, 0));
	mCharacter->SetVelocity(VECTOR3F(0, 0, 0));
	mCharacter->SetAngle(VECTOR3F(0, 0, 0));
	mCharacter->CalculateBoonTransform(0);
	mCharacter->SetAccel(VECTOR3F(0, 0, 0));
	mGravity = mParameter.gravity;
	camera->Reset();
	mCharacter->SetExist(true);
}
