#include "title_camera_operation.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif

TitleCameraOperation::TitleCameraOperation()
	: mTitleSceneChangeFlag(false), mTime(0), mEndTitleFlag(false), mLerpMovement(0)
{
}

void TitleCameraOperation::ImGuiUpdate(Camera* camera)
{
#ifdef USE_IMGUI
	ImGui::Begin("title camera");
	ImGui::InputFloat("startEye.x", &mTitleData.mEye.x, 1);
	ImGui::InputFloat("startEye.y", &mTitleData.mEye.y, 1);
	ImGui::InputFloat("startEye.z", &mTitleData.mEye.z, 1);
	ImGui::SliderFloat("startFront.x", &mTitleData.mFront.x, -1, 1);
	ImGui::SliderFloat("startFront.y", &mTitleData.mFront.y, -1, 1);
	ImGui::SliderFloat("startFront.z", &mTitleData.mFront.z, -1, 1);
	ImGui::SliderFloat("lerpMin", &mTitleData.mMinLerp, 0, 1);
	ImGui::SliderFloat("lerpMax", &mTitleData.mMaxLerp, 0, 1);
	ImGui::InputFloat("startTime", &mTitleData.startTime, 0.1f);
	ImGui::SliderFloat("lerp change amount", &mTitleData.mLerpChangeAmount, 0, 1);
	if (ImGui::Button("save"))
	{
		Save();
	}

	ImGui::End();
#endif
}

void TitleCameraOperation::Update(Camera* camera, float elapsedTime)
{
	if (!mTitleSceneChangeFlag)
	{
		camera->SetEye(mTitleData.mEye);
		camera->SetFocus(mTitleData.mEye + mTitleData.mFront);

		return;//spaceキーが押されたかどうか
	}
	mTime += elapsedTime;
	//押されてから一定時間がたったかどうか
	if (mTime >= mTitleData.startTime)
	{
		VECTOR3F eye = camera->GetEye();
		DirectX::XMVECTOR eyeVecc = DirectX::XMLoadFloat3(&eye);
		DirectX::XMVECTOR endPosVecc = DirectX::XMLoadFloat3(&mTitleData.endPosition);
		float length = 0;
		DirectX::XMStoreFloat(&mLerpMovement, DirectX::XMVectorLerp(DirectX::XMLoadFloat(&mTitleData.mMinLerp), DirectX::XMLoadFloat(&mTitleData.mMaxLerp), mTitleData.mLerpChangeAmount * 60 * elapsedTime));
		//カメラのeye座標からendPosition(最終座標)までlerp関数で移動する
		eyeVecc = DirectX::XMVectorLerp(eyeVecc, endPosVecc, mLerpMovement*60*elapsedTime);
		//カメラのeye座標からendPosition(最終座標)までの距離を測る
		DirectX::XMStoreFloat(&length, DirectX::XMVector3Length(DirectX::XMVectorSubtract(eyeVecc, endPosVecc)));
		DirectX::XMStoreFloat3(&eye, eyeVecc);
		camera->SetEye(eye);
		camera->SetFocus(eye + mTitleData.mFront);
		//距離が一定以下かどうか
		if (length <= 20)
		{
			mEndTitleFlag = true;
		}
	}

}

void TitleCameraOperation::Load(Camera* camera)
{
	FILE* fp;
	if (fopen_s(&fp, "Data/file/Title_camera.bin", "rb") == 0)
	{
		fread(&mTitleData, sizeof(TitleCameraData), 1, fp);
		fclose(fp);
	}
	else
	{
		mTitleData.mEye = camera->GetEye();
		VECTOR3F front = camera->GetFocus() - mTitleData.mEye;
		DirectX::XMStoreFloat3(&mTitleData.mFront, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&front)));
		mTitleData.endPosition = VECTOR3F(0, 0, 100);
	}

}

void TitleCameraOperation::Save()
{
	FILE* fp;
	fopen_s(&fp, "Data/file/Title_camera.bin", "wb");

	fwrite(&mTitleData, sizeof(TitleCameraData), 1, fp);
	fclose(fp);

}
