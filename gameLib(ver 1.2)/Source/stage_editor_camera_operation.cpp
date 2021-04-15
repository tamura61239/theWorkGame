#include "stage_editor_camera_operation.h"
#include"static_obj.h"
#include"key_board.h"
#include<math.h>
#ifdef USE_IMGUI
#include<imgui.h>
#endif
//�R���X�g���N�^
StageEditorCameraOperation::StageEditorCameraOperation(Camera* camera)
	:mNewPosition(camera->GetFocus()), mMoveFlag(false)
{
	mData.mAngleX = DirectX::XMConvertToRadians(10);
	mData.mAngleY = DirectX::XMConvertToRadians(90);
	mData.r = 100;
}
/********************�G�f�B�^�֐�************************/
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
/************************�X�V�֐�**************************/
void StageEditorCameraOperation::Update(Camera* camera,float elapsedTime)
{
	//�J��������p�����[�^�[���擾
	VECTOR3F eye = camera->GetEye();
	VECTOR3F focus = camera->GetFocus();

	//���W���X�V����Ă�����
	if (mMoveFlag)
	{
		//�J�����̒����_�ƐV�������W�Ƌ������v�Z����
		float length = 0;
		DirectX::XMVECTOR  f = DirectX::XMLoadFloat3(&focus);
		DirectX::XMVECTOR  n = DirectX::XMLoadFloat3(&mNewPosition);
		DirectX::XMStoreFloat(&length, DirectX::XMVector3Length(DirectX::XMVectorSubtract(f, n)));
		if (length >= 0.1f)
		{//�J�����̒����_��V�������W�Ɋ񂹂Ă���
			DirectX::XMStoreFloat3(&focus, DirectX::XMVectorLerp(f, n, elapsedTime * 3));
		}
		else
		{//���ȏ�񂹂���I��
			focus = mNewPosition;
			mMoveFlag = false;
		}
	}
	else
	{
		//�X�V����ĂȂ����L�[�ŃJ�����̒����_�𓮂�����
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
	//�G�f�B�^�ł���������]����sin,cos���v�Z����
	float sx = ::sinf(mData.mAngleX);
	float cx = ::cosf(mData.mAngleX);
	float sy = ::sinf(mData.mAngleY);
	float cy = ::cosf(mData.mAngleY);

	VECTOR3F vec = VECTOR3F(0, 0, 0);
	//�v�Z����sin,cos����J�������W�����߂�
	DirectX::XMVECTOR front = DirectX::XMVectorSet(cx * sy, sx, cx * cy, 0.0f);
	DirectX::XMVECTOR right = DirectX::XMVectorSet(cy, 0, -sy, 0.0f);
	DirectX::XMVECTOR up = DirectX::XMVector3Cross(right, front);
	DirectX::XMStoreFloat3(&vec, front);
	eye = focus + vec * mData.r;
	//�J�����̃p�����[�^�[���Z�b�g����
	camera->SetEye(eye);
	camera->SetFocus(focus);
}

