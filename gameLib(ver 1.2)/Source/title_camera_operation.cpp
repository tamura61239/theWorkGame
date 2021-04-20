#include "title_camera_operation.h"
#include"file_function.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�������֐�(�R���X�g���N�^)
/*****************************************************/
TitleCameraOperation::TitleCameraOperation(Camera* camera)
	: mTitleSceneChangeFlag(false), mTime(0), mEndTitleFlag(false), mLerpMovement(0)
{
	mTitleData.mEye = camera->GetEye();
	VECTOR3F front = camera->GetFocus() - mTitleData.mEye;
	DirectX::XMStoreFloat3(&mTitleData.mFront, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&front)));
	mTitleData.endPosition = VECTOR3F(0, 0, 100);
	FileFunction::Load(mTitleData, "Data/file/Title_camera.bin", "rb");
}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�G�f�B�^�֐�
/*****************************************************/
void TitleCameraOperation::Editor(Camera* camera)
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
		FileFunction::Save(mTitleData, "Data/file/Title_camera.bin", "wb");
	}

	ImGui::End();
#endif
}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�X�V�֐�
/*****************************************************/
void TitleCameraOperation::Update(Camera* camera, float elapsedTime)
{
	//space�L�[�������ꂽ���ǂ���
	if (!mTitleSceneChangeFlag)
	{
		camera->SetEye(mTitleData.mEye);
		camera->SetFocus(mTitleData.mEye + mTitleData.mFront);

		return;
	}
	mTime += elapsedTime;
	//������Ă����莞�Ԃ����������ǂ���
	if (mTime >= mTitleData.startTime)
	{
		VECTOR3F eye = camera->GetEye();
		DirectX::XMVECTOR eyeVecc = DirectX::XMLoadFloat3(&eye);
		DirectX::XMVECTOR endPosVecc = DirectX::XMLoadFloat3(&mTitleData.endPosition);
		float length = 0;
		DirectX::XMStoreFloat(&mLerpMovement, DirectX::XMVectorLerp(DirectX::XMLoadFloat(&mTitleData.mMinLerp), DirectX::XMLoadFloat(&mTitleData.mMaxLerp), mTitleData.mLerpChangeAmount * 60 * elapsedTime));
		//�J������eye���W����endPosition(�ŏI���W)�܂�lerp�֐��ňړ�����
		eyeVecc = DirectX::XMVectorLerp(eyeVecc, endPosVecc, mLerpMovement*60*elapsedTime);
		//�J������eye���W����endPosition(�ŏI���W)�܂ł̋����𑪂�
		DirectX::XMStoreFloat(&length, DirectX::XMVector3Length(DirectX::XMVectorSubtract(eyeVecc, endPosVecc)));
		DirectX::XMStoreFloat3(&eye, eyeVecc);
		camera->SetEye(eye);
		camera->SetFocus(eye + mTitleData.mFront);
		//���������ȉ����ǂ���
		if (length <= 20)
		{
			mEndTitleFlag = true;
		}
	}

}
