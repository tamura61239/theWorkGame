#include "stage_editor.h"
#include"framework.h"
#include<math.h>
#include"key_board.h"
#include"camera_manager.h"

/********************������*********************/
//�R���X�g���N�^
StageEditor::StageEditor(ID3D11Device* device, int width, int height)
	:mMousePosition(0, 0), mWorldFarPosition(0, 0, 0), mWorldNearPosition(0, 0, 0)
	, mStageLeftPosition(0, 0, 0), mStageRightPosition(0, 0, 0), mWidth(width), mHeight(height)
	, mSidoViewRenderPosition(1344, 756), mSidoViewRenderSize(576, 324), mEditorFlag(false)
	, mDragObjNo(-1), mFileState(0), mEditorState(0),mCreateFlag(false), mDeleteNum(-1), mSaveSceneFlag(false)
{
	mSprite = std::make_unique<Sprite>(device);
	mStageSidoView = std::make_unique<FrameBuffer>(device, 1920, 1080, true, 8, DXGI_FORMAT_R8G8B8A8_UNORM);
	mSidoCamera = std::make_unique<Camera>(device);
	mSidoCamera->SetPerspective(30 * (3.14f / 180.f), static_cast<float>(mWidth) / static_cast<float>(mHeight), 0.1f, 100000);
	mObj = std::make_unique<Obj3D>();
	ClearCreateData();
}
void StageEditor::ClearCreateData()
{
	mCreateData.mAngle = VECTOR3F(0, 0, 0);
	mCreateData.mPosition = pCameraManager->GetCamera()->GetFocus();
	mCreateData.mScale = VECTOR3F(1, 1, 1);
	mCreateData.mColorType = 0;
	mCreateData.mObjType = 0;
	mObj->SetAngle(mCreateData.mAngle);
	mObj->SetPosition(mCreateData.mPosition);
	mObj->SetScale(mCreateData.mScale);
	mCreateFlag = false;

}
/****************�X�V**********************/
//�G�f�B�^�֐�
int StageEditor::Update(std::vector<std::shared_ptr<StageObj>>objs)
{
#ifdef USE_IMGUI
	mDeleteNum = -1;
	UpdateMouseData();
	//MouseToWorld();
	ImGui::Begin("stage editor");
	if (ImGui::CollapsingHeader("editor mode"))
	{
		mEditorFlag = true;
		pCameraManager->GetCameraOperation()->SetCameraType(CameraOperation::CAMERA_TYPE::STAGE_EDITOR);
		//���N���b�N�������Ƃ�
		if (pKeyBoad.RisingState(VK_LBUTTON))
		{
			if (MouseJudg())
			{
				NewCameraPosition();
			}
			else if(pKeyBoad.PressedState(KeyLabel::LCONTROL))
			{
				SearchStageObj(objs);
			}
		}
		//StageEditor�̑��쒆�̃J����
		pCameraManager->ImGuiUpdate();
		//�h���b�O����I�u�W�F�N�g�̑���
		if (mDragObjNo != -1)
		{
			auto& obj = objs[mDragObjNo];

			mEditorState = 1;
			float* position[3] = { &obj->GetPosition().x,&obj->GetPosition().y ,&obj->GetPosition().z };
			ImGui::DragFloat3("position", *position);
			float* angle[3] = { &obj->GetAngle().x,&obj->GetAngle().y ,&obj->GetAngle().z };
			ImGui::SliderFloat3("angle", *angle, -3.14f, 3.14f);
			float* scale[3] = { &obj->GetScale().x,&obj->GetScale().y ,&obj->GetScale().z };
			ImGui::DragFloat3("scale", *scale);
			int colorType = obj->GetStageData().mColorType;
			ImGui::SliderInt("color type", &colorType, 0, 1);
			obj->SetColorType(colorType);
			if (ImGui::Button("delete"))
			{
				mDeleteNum = mDragObjNo;
				mDragObjNo = -1;
			}
			if (ImGui::Button("end"))
			{
				mDragObjNo = -1;
			}
			StageData stageData;
			static float s = gameObjScale / 10.0f;

			stageData.mPosition = obj->GetPosition()/ s;
			stageData.mAngle = obj->GetAngle();
			stageData.mScale = obj->GetScale()/ s;
			stageData.mColorType = obj->GetStageData().mColorType;
			stageData.mObjType = obj->GetStageData().mObjType;
			obj->SetStageData(stageData);
		}
		else//obj���h���b�O���Ȃ���
		{
			mEditorState = 3;
			if (ImGui::CollapsingHeader("create data set"))
			{
				mEditorState = 2;

				float position[3] = { mObj->GetPosition().x,mObj->GetPosition().y ,mObj->GetPosition().z };
				ImGui::DragFloat3("position", position);
				float angle[3] = { mObj->GetAngle().x,mObj->GetAngle().y ,mObj->GetAngle().z };
				ImGui::DragFloat3("angle", angle,DirectX::XMConvertToRadians(1), -3.14f, 3.14f);
				float scale[3] = { mObj->GetScale().x,mObj->GetScale().y ,mObj->GetScale().z };
				ImGui::DragFloat3("scale", scale);
				ImGui::SliderInt("color type", &mCreateData.mColorType, 0, 1);
				ImGui::SliderInt("obj type", &mCreateData.mObjType, 0, 2);
				if (ImGui::Button("create"))
				{
					CreateDataSetTransformData();
				}
				mObj->SetAngle(VECTOR3F(angle[0], angle[1], angle[2]));
				mObj->SetPosition(VECTOR3F(position[0], position[1], position[2]));
				mObj->SetScale(VECTOR3F(scale[0], scale[1], scale[2]));
				mObj->CalculateTransform();
			}
			else
			{
				mObj->SetPosition(pCameraManager->GetCamera()->GetFocus());
			}
		}
		ImGui::Separator();
		if (ImGui::Button("save"))
		{
			mFileState = 2;
		}
		if (ImGui::Button("load"))
		{
			mFileState = 1;
		}
		mSaveSceneFlag = false;
		if (ImGui::Button("stage screne shot"))
		{
			mSaveSceneFlag = true;
		}
		SetSidoCamera(objs);

	}
	else
	{
		mEditorFlag = false;
		mEditorState = 0;
		if (pCameraManager->GetCameraOperation()->GetCameraType() == CameraOperation::CAMERA_TYPE::STAGE_EDITOR)
		{
			pCameraManager->GetCameraOperation()->SetCameraType(CameraOperation::CAMERA_TYPE::PLAY);
		}
	}
	ImGui::Text("particle %d", objs.size());
	ImGui::End();
#endif

	return -1;
}
//�}�E�X�X�V
void StageEditor::UpdateMouseData()
{
	POINT cursor;
	GetCursorPos(&cursor);

	ScreenToClient(Framework::Instance().GetHwnd(), &cursor);
	mMousePosition = VECTOR2F(static_cast<float>(cursor.x), static_cast<float>(cursor.y));
	mMousePosition.x += 10;
	mMousePosition.y += 10;
}
//�X�N���[�����W�����[���h���W�ւ̕ϊ��֐�
void StageEditor::ScreeenToWorld(VECTOR3F* worldPosition, const VECTOR3F& screenPosition, Camera* camera)
{
	float viewportX = 0.0f;
	float viewportY = 0.0f;
	float viewportW = static_cast<float>(mWidth);
	float viewportH = static_cast<float>(mHeight);
	float viewportMinZ = 0.0f;
	float viewportMaxZ = 1.0f;
	// �r���[�s��
	DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&camera->GetView());
	// �v���W�F�N�V�����s��
	DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&camera->GetProjection());
	// ���[���h�s��
	DirectX::XMMATRIX W = DirectX::XMMatrixIdentity();//�ړ������͂���Ȃ��̂ŒP�ʍs������Ă����B
	// �X�N���[�����W����NDC���W�֕ϊ�
	DirectX::XMVECTOR NDCPos = DirectX::XMVectorSet(
		2.0f * (screenPosition.x - viewportX) / viewportW - 1.0f,
		1.0f - 2.0f * (screenPosition.y - viewportY) / viewportH,
		(screenPosition.z - viewportMinZ) / (viewportMaxZ - viewportMinZ),
		1.0f);
	// NDC���W���烏�[���h���W�֕ϊ�
	DirectX::XMMATRIX WVP = W * V * P;
	DirectX::XMMATRIX IWVP = DirectX::XMMatrixInverse(nullptr, WVP);
	DirectX::XMVECTOR WPos = DirectX::XMVector3TransformCoord(NDCPos, IWVP);
	DirectX::XMStoreFloat3(worldPosition, WPos);

}

void StageEditor::MouseToWorld(Camera* camera, const VECTOR2F& mousePosition)
{
	VECTOR3F positionNear = VECTOR3F(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y), 0);
	VECTOR3F positionFar = VECTOR3F(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y), 1);

	ScreeenToWorld(&mWorldNearPosition, positionNear, camera);
	ScreeenToWorld(&mWorldFarPosition, positionFar, camera);
}
//�T�C�h�J�����s��v�Z�֐�
void StageEditor::SetSidoCamera(std::vector<std::shared_ptr<StageObj>>objs)
{
	float minZ = 0, maxZ = 0;
	float minY = 0, maxY = 0;
	//�X�e�[�W�̃I�u�W�F�N�g��z����y���̍ő�l�ƍŏ��l
	for (auto& obj : objs)
	{
		VECTOR3F stageMin = obj->GetPosition() - obj->GetScale();
		VECTOR3F stageMax = obj->GetPosition() + obj->GetScale();
		if (stageMin.z < minZ)
		{
			minZ = stageMin.z;
		}
		if (stageMin.y < minY)
		{
			minY = stageMin.y;
		}
		if (stageMax.z > maxZ)
		{
			maxZ = stageMax.z;
		}
		if (stageMax.y > maxY)
		{
			maxY = stageMax.y;
		}

	}
	//�����̑傫���ق��̔��a
	float r = 0;
	float rZ = maxZ - minZ;
	float rY = maxY - minY;
	r = (rZ > rY) ? rZ : rY;
	r *= 0.5f;
	float fov = pCameraManager->GetCamera()->GetFov();
	float aspect = pCameraManager->GetCamera()->GetAspect();

	float theta = (aspect >= 1.0f) ? fov : fov * aspect;
	//�J�����̋����v�Z
	float length = r / sinf(theta / 2);
	//�ݒ�
	mSidoCamera->SetFocus(VECTOR3F(0, minY, minZ) + VECTOR3F(0, rY, rZ) * 0.5f);
	mSidoCamera->SetEye(mSidoCamera->GetFocus() + VECTOR3F(length, 0, 0));
	//�s��v�Z
	mSidoCamera->CalculateMatrix();
}

void StageEditor::SearchStageObj(std::vector<std::shared_ptr<StageObj>>objs)
{
	if (mDragObjNo == -1)
	{
		MouseToWorld(pCameraManager->GetCamera(), mMousePosition);
		mDragObjNo = NewDragObj(objs);

	}
}

bool StageEditor::MouseJudg()
{
	if (mMousePosition.x > mSidoViewRenderPosition.x && mMousePosition.x < mSidoViewRenderPosition.x + mSidoViewRenderSize.x)
	{
		if (mMousePosition.y > mSidoViewRenderPosition.y && mMousePosition.y < mSidoViewRenderPosition.y + mSidoViewRenderSize.y)
		{
			return true;
		}
	}
	return false;
}

int StageEditor::NewDragObj(std::vector<std::shared_ptr<StageObj>>objs)
{
	float length = FLT_MAX;
	int objNo = -1;
	for (int i = 0; i < static_cast<int>(objs.size()); i++)
	{
		auto& obj = objs[i];
		VECTOR3F position, normal;
		float outLength = 0;
		if (obj->RayPick(mWorldNearPosition, mWorldFarPosition, &position, &normal, &outLength) != -1)
		{
			if (outLength < length)
			{
				length = outLength;
				objNo = i;
			}
		}
	}
	return objNo;
}

void StageEditor::NewCameraPosition()
{
	if (pCameraManager->GetCameraOperation()->GetStageEditorCamera()->GetMoveFlag())return;
	VECTOR2F textMagnification = VECTOR2F(static_cast<float>(mWidth), static_cast<float>(mHeight)) / mSidoViewRenderSize;
	VECTOR2F textPosition = mMousePosition - mSidoViewRenderPosition;
	VECTOR2F position = textPosition * textMagnification;
	MouseToWorld(mSidoCamera.get(), position);

	VECTOR3F vec;

	DirectX::XMStoreFloat3(&vec, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&VECTOR3F(mWorldFarPosition - mWorldNearPosition))));

	float z = 0 - mWorldNearPosition.x;
	float length = (z / vec.x);

	pCameraManager->GetCameraOperation()->GetStageEditorCamera()->SetNewPosition(mWorldNearPosition + vec * length);


}
/*****************�`��*********************/
void StageEditor::Render(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection)
{
}

//�T�C�h�V�[���̕`��
void StageEditor::SidoViewRender(ID3D11DeviceContext* context)
{
	if (!mEditorFlag)return;
	mSprite->Render(context, mStageSidoView->GetRenderTargetShaderResourceView().Get(), mSidoViewRenderPosition, mSidoViewRenderSize, VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
}

void StageEditor::EditorCreateObjImageRender(ID3D11DeviceContext* context, StaticMesh* mesh, MeshRender* render, VECTOR4F color)
{
	if (mEditorState != 2)return;
	render->Render(context, mesh, mObj->GetWorld(), color);
}
