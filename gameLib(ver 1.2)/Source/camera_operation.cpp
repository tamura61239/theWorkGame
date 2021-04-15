#include "camera_operation.h"
#include<Windows.h>
#include<string>
#ifdef USE_IMGUI
#include <imgui.h>
#endif

//�R���X�g���N�^
CameraOperation::CameraOperation(Camera* camera, int scene) :mType(CAMERA_TYPE::NORMAL), mScene(scene)
{
	VECTOR3F focusF = camera->GetFocus();
	VECTOR3F eyeF = camera->GetEye();
	VECTOR3F l = focusF - eyeF;
	distance = sqrtf(l.x * l.x + l.y * l.y + l.z * l.z);
	//�V�[���ɍ��킹������N���X�̐���
	switch (mScene)
	{
	case 0://�^�C�g��
		mTitleCamera = std::make_unique<TitleCameraOperation>(camera);
		break;
	case 1://�Q�[��
		mPlayCamera = std::make_unique<PlayCameraOperation>();
		mStageEditorCamera = std::make_unique<StageEditorCameraOperation>(camera);
		break;
	}
}
/************************�G�f�B�^�֐�*****************************/
void CameraOperation::Editor(Camera* camera)
{
#ifdef USE_IMGUI
	ImGui::Begin("camera");
	//�J��������N���X�̃G�f�B�^��I������
	int type = static_cast<int>(mType);
	ImGui::RadioButton("normal", &type, 0);
	ImGui::RadioButton("debug", &type, 1);
	switch (mScene)
	{
	case 0:
		ImGui::RadioButton("title camera", &type, 2);
		break;
	case 1:
		ImGui::RadioButton("play camera", &type, 3);
		ImGui::RadioButton("stage editor camera", &type, 4);
		break;
	}
	if (mType != static_cast<CAMERA_TYPE>(type))
	{
		//�f�o�b�N�J��������߂鎞��x�N�g�������ɖ߂�
		if (mType == CAMERA_TYPE::DEBUG)
		{
			camera->SetUp(VECTOR3F(0, 1, 0));
		}
	}
	mType = static_cast<CAMERA_TYPE>(type);
	ImGui::Separator();
	VECTOR3F eye = camera->GetEye();
	VECTOR3F focus = camera->GetFocus();
	VECTOR3F up = camera->GetUp();
	//�J�����̃p�����[�^�[�̕\��
	ImGui::Text("[eye] x:%f y:%f z:%f", eye.x, eye.y, eye.z);
	ImGui::Text("[focus] x:%f y:%f z:%f", focus.x, focus.y, focus.z);
	ImGui::Text("[up] x:%f y:%f z:%f", up.x, up.y, up.z);
	ImGui::Separator();
	//�I�����ꂽ�J��������N���X�̃G�f�B�^�֐����Ă�
	switch (mType)
	{
	case CAMERA_TYPE::NORMAL:
		break;
	case CAMERA_TYPE::DEBUG:
		break;
	case CAMERA_TYPE::TITLE_CAMERA:
		mTitleCamera->Editor(camera);
		break;
	case CAMERA_TYPE::PLAY:
		mPlayCamera->Editor();
		break;
	case CAMERA_TYPE::STAGE_EDITOR:
		mStageEditorCamera->Editor(camera);
		break;
	}
	ImGui::End();
#endif

}
/****************************�X�V�֐�******************************/
void CameraOperation::Update(Camera* camera,float elapsedTime)
{

	switch (mType)
	{
	case CAMERA_TYPE::DEBUG:
		DebugCamera(camera);
		break;
	case CAMERA_TYPE::TITLE_CAMERA:
		mTitleCamera->Update(camera,elapsedTime);
		break;
	case CAMERA_TYPE::PLAY:
		mPlayCamera->Update(camera,elapsedTime);
		break;
	case CAMERA_TYPE::STAGE_EDITOR:
		mStageEditorCamera->Update(camera,elapsedTime);
		break;
	}
}
//�f�o�b�N�J����
void CameraOperation::DebugCamera(Camera* camera)
{
	//�J�������̎擾
	VECTOR3F focusF = camera->GetFocus();
	VECTOR3F upF = camera->GetUp();
	VECTOR3F rightF = camera->GetRight();
	VECTOR3F eyeF = camera->GetEye();
	//�}�E�X���W�̎擾
	POINT cursor;
	::GetCursorPos(&cursor);
	//�O�̃t���[���̃}�E�X���W��ۑ�
	oldCursor = newCursor;
	//�V�����}�E�X���W���Z�b�g
	newCursor = VECTOR2F(static_cast<float>(cursor.x), static_cast<float>(cursor.y));

	//�ړ��ʂ��v�Z
	float move_x = (newCursor.x - oldCursor.x) * 0.02f;
	float move_y = (newCursor.y - oldCursor.y) * 0.02f;

	// Alt�L�[
	if (::GetAsyncKeyState(VK_MENU) & 0x8000)
	{
		//���N���b�N
		if (::GetAsyncKeyState(VK_LBUTTON) & 0x8000)
		{
			// Y����]
			rotate.y += move_x * 0.5f;
			if (rotate.y > DirectX::XM_PI)
			{
				rotate.y -= DirectX::XM_2PI;
			}
			else if (rotate.y < -DirectX::XM_PI)
			{
				rotate.y += DirectX::XM_2PI;
			}
			// X����]
			rotate.x += move_y * 0.5f;
			if (rotate.x > DirectX::XM_PI)
			{
				rotate.x -= DirectX::XM_2PI;
			}
			else if (rotate.x < -DirectX::XM_PI)
			{
				rotate.x += DirectX::XM_2PI;
			}
		}
		else if (::GetAsyncKeyState(VK_MBUTTON) & 0x8000)
		{
			// ���s�ړ�
			float s = distance * 0.035f;
			float x = -move_x * s;
			float y = move_y * s;

			focusF.x += rightF.x * x;
			focusF.y += rightF.y * x;
			focusF.z += rightF.z * x;

			focusF.x += upF.x * y;
			focusF.y += upF.y * y;
			focusF.z += upF.z * y;
		}
		//�E�N���b�N
		else if (::GetAsyncKeyState(VK_RBUTTON) & 0x8000)
		{
			// �Y�[��
			distance += (-move_y - move_x) * distance * 0.1f;
		}

		//��]��sin,cos����
		float sx = ::sinf(rotate.x);
		float cx = ::cosf(rotate.x);
		float sy = ::sinf(rotate.y);
		float cy = ::cosf(rotate.y);

		//�v�Z����sin,cos���g���đO�x�N�g���A�E�x�N�g���A��x�N�g�����o��
		DirectX::XMVECTOR front = DirectX::XMVectorSet(-cx * sy, -sx, -cx * cy, 0.0f);
		DirectX::XMVECTOR right = DirectX::XMVectorSet(cy, 0, -sy, 0.0f);
		DirectX::XMVECTOR up = DirectX::XMVector3Cross(right, front);

		//�O�x�N�g���Ƌ����ƒ����_(focus)���g���ăJ�������W(eye)�����߂�
		DirectX::XMVECTOR focus = DirectX::XMLoadFloat3(&focusF);
		DirectX::XMVECTOR distance = DirectX::XMVectorSet(this->distance, this->distance, this->distance, 0.0f);
		DirectX::XMVECTOR eye = DirectX::XMVectorSubtract(focus, DirectX::XMVectorMultiply(front, distance));
		DirectX::XMStoreFloat3(&eyeF, eye);
		DirectX::XMStoreFloat3(&upF, up);
		//�o���l���J�����ɃZ�b�g����
		camera->SetEye(eyeF);
		camera->SetFocus(focusF);
		camera->SetUp(upF);
	}
}
