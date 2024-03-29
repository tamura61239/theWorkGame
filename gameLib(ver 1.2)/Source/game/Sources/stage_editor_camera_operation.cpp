#include "stage_editor_camera_operation.h"
#include"static_obj.h"
#include"key_board.h"
#include<math.h>
#ifdef USE_IMGUI
#include<imgui.h>
#endif
/*****************************************************/
//　　　　　　　　　　初期化関数(コンストラクタ)
/*****************************************************/
StageEditorCameraOperation::StageEditorCameraOperation(Camera* camera)
	:mNewPosition(camera->GetFocus()), mMoveFlag(false)
{
	mData.mAngleX = DirectX::XMConvertToRadians(10);
	mData.mAngleY = DirectX::XMConvertToRadians(90);
	mData.r = 100;
}
/*****************************************************/
//　　　　　　　　　　エディタ関数
/*****************************************************/
void StageEditorCameraOperation::Editor(Camera* camera)
{
#ifdef USE_IMGUI
	VECTOR3F focus = camera->GetFocus();
	ImGui::Begin("stage editor camera");
	ImGui::SliderFloat("angle y", &mData.mAngleY,-3.14f,3.14f);
	ImGui::SliderFloat("angle x", &mData.mAngleX, -1.5f, 1.5f);
	ImGui::InputFloat("r", &mData.r, 1);
	ImGui::InputFloat("focus z", &focus.z, 1);
	ImGui::InputFloat("focus y", &focus.y, 1);
	ImGui::End();
	if (mMoveFlag)return;
	camera->SetFocus(focus);
#endif
}
/*****************************************************/
//　　　　　　　　　　更新関数
/*****************************************************/
void StageEditorCameraOperation::Update(Camera* camera,float elapsedTime)
{
	//カメラからパラメーターを取得
	VECTOR3F eye = camera->GetEye();
	VECTOR3F focus = camera->GetFocus();

	//座標が更新されていたら
	if (mMoveFlag)
	{
		//カメラの注視点と新しい座標と距離を計算する
		float length = 0;
		DirectX::XMVECTOR  f = DirectX::XMLoadFloat3(&focus);
		DirectX::XMVECTOR  n = DirectX::XMLoadFloat3(&mNewPosition);
		DirectX::XMStoreFloat(&length, DirectX::XMVector3Length(DirectX::XMVectorSubtract(f, n)));
		if (length >= 0.1f)
		{//カメラの注視点を新しい座標に寄せていく
			DirectX::XMStoreFloat3(&focus, DirectX::XMVectorLerp(f, n, elapsedTime * 3));
		}
		else
		{//一定以上寄せたら終了
			focus = mNewPosition;
			mMoveFlag = false;
		}
	}
	else
	{
		//更新されてない時キーでカメラの注視点を動かせる
		if (pKeyBoad.PressedState(KeyLabel::A))
		{
			focus.z -= 100 * elapsedTime;
		}
		if (pKeyBoad.PressedState(KeyLabel::D))
		{
			focus.z += 100 * elapsedTime;

		}
		if (pKeyBoad.PressedState(KeyLabel::W))
		{
			focus.y += 100 * elapsedTime;
		}
		if (pKeyBoad.PressedState(KeyLabel::S))
		{
			focus.y -= 100 * elapsedTime;

		}

	}
	//エディタでいじった回転からsin,cosを計算する
	float sx = ::sinf(mData.mAngleX);
	float cx = ::cosf(mData.mAngleX);
	float sy = ::sinf(mData.mAngleY);
	float cy = ::cosf(mData.mAngleY);

	VECTOR3F vec = VECTOR3F(0, 0, 0);
	//計算したsin,cosからカメラ座標を求める
	DirectX::XMVECTOR front = DirectX::XMVectorSet(cx * sy, sx, cx * cy, 0.0f);
	DirectX::XMVECTOR right = DirectX::XMVectorSet(cy, 0, -sy, 0.0f);
	DirectX::XMVECTOR up = DirectX::XMVector3Cross(right, front);
	DirectX::XMStoreFloat3(&vec, front);
	eye = focus + vec * mData.r;
	//カメラのパラメーターをセットする
	camera->SetEye(eye);
	camera->SetFocus(focus);
}

