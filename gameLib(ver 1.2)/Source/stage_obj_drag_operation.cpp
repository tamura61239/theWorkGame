#include "stage_obj_drag_operation.h"
#include"key_board.h"
#include"camera_manager.h"
#include"framework.h"

StageObjDragOperation::StageObjDragOperation(ID3D11Device* device)
{
	std::shared_ptr<StaticMesh>mesh;
	mesh = std::make_shared<StaticMesh>(device, "Data/FBX/Arrow/arrow.fbx");
	for (int i = 0;i < 3;i++)
	{
		mArrowObjs[i] = std::make_shared<StaticObj>(mesh);
		mArrowObjs[i]->SetScale(VECTOR3F(0, 0, 0));
		mArrowObjs[i]->SetColor(VECTOR4F(0,0,0,0));
	}
	mArrowObjs[0]->SetAngle(VECTOR3F(0, 0, DirectX::XMConvertToRadians(90.f)));
	mArrowObjs[1]->SetAngle(VECTOR3F(0, 0, DirectX::XMConvertToRadians(180.f)));
	mArrowObjs[2]->SetAngle(VECTOR3F(DirectX::XMConvertToRadians(90.f), 0, 0));
	mArrowSize = mesh->GetMaxPosition() - mesh->GetMinPosition();
	mLocalPosition[0] = VECTOR3F(1, 0.1, 0);
	mLocalPosition[1] = VECTOR3F(0, 1.1, 0);
	mLocalPosition[2] = VECTOR3F(0, 0.1, -1);
	for (int i = 0;i < 3;i++)
	{
		//mLocalPosition[i] *= mArrowSize.y;
		mLocalScale[i] = VECTOR3F(0, 0, 0);
	}
}

void StageObjDragOperation::DragStart(const VECTOR3F& farMouse, const VECTOR3F& min, const VECTOR3F& max)
{
	mBeforeFar = farMouse;
	mDragObjSize = max - min;
}

int StageObjDragOperation::Update(std::shared_ptr<StageObj> obj, const VECTOR3F& nearMouse, const VECTOR3F& farMouse, int number)
{
#ifdef USE_IMGUI
	ImGui::Begin("drag");
	//float* dragScale0[3] = { &mLocalScale[0].x,&mLocalScale[0].y ,&mLocalScale[0].z };
	//float* dragScale1[3] = { &mLocalScale[1].x,&mLocalScale[1].y ,&mLocalScale[1].z };
	//float* dragScale2[3] = { &mLocalScale[2].x,&mLocalScale[2].y ,&mLocalScale[2].z };
	//ImGui::SliderFloat3("scale0", *dragScale0, 0, 10);
	//ImGui::SliderFloat3("scale1", *dragScale1, 0, 10);
	//ImGui::SliderFloat3("scale2", *dragScale2, 0, 10);
	VECTOR3F p = obj->GetPosition();
	ImGui::SliderFloat("pos.,z", &p.z, -1000, 1000);
	obj->SetPosition(p);
	if (ImGui::Button("end"))
	{
		for (int i = 0;i < 3;i++)
		{
			mArrowObjs[i]->SetColor(VECTOR4F(0, 0, 0, 0));
		}
		ImGui::End();
		return -1;
	}
	ImGui::End();
#endif
	SetArrow(obj);
	if (pKeyBoad.PressedState(VK_LBUTTON)&&!pKeyBoad.PressedState(VK_MENU))
	{
		int aroowNumber = isHitArrow(nearMouse, farMouse);
		if (aroowNumber > -1)
		{
			Move(obj, farMouse, aroowNumber);
			VECTOR4F color = mArrowObjs[aroowNumber]->GetColor();
			mArrowObjs[aroowNumber]->SetColor(VECTOR4F(color.x, color.y, color.z, 1));
		}
	}
	mBeforeFar = farMouse;
	return number;
}

int StageObjDragOperation::isHitArrow(const VECTOR3F& nearMouse, const VECTOR3F& farMouse)
{
	int number = -1;
	float minLength = 10000;
	VECTOR3F position, normal;
	for (int i = 0;i < 3;i++)
	{
		float length;
		if (mArrowObjs[i]->RayPick(nearMouse, farMouse, &position, &normal, &length) != -1)
		{
			if (minLength > length)
			{
				minLength = length;
				number = i;
			}
		}
	}
	return number;
}

void StageObjDragOperation::Move(std::shared_ptr<StageObj> obj, const VECTOR3F& farMouse, int arrowNumber)
{
	float cameraFar = pCamera.GetCamera()->GetFar();
	VECTOR3F objPosition = obj->GetPosition();
	DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&objPosition);
	DirectX::XMVECTOR eye = DirectX::XMLoadFloat3(&pCamera.GetCamera()->GetEye());
	DirectX::XMVECTOR length = DirectX::XMVector3Length(DirectX::XMVectorSubtract(position, eye));

	float fLength;
	DirectX::XMStoreFloat(&fLength, length);
	VECTOR3F velocity = VECTOR3F(0, 0, 0);

	switch (arrowNumber)
	{
	case 0:
		velocity.x = (farMouse.x - mBeforeFar.x) / cameraFar * fLength;
		break;
	case 1:
		velocity.y = (farMouse.y - mBeforeFar.y) / cameraFar * fLength;
		break;
	case 2:
		velocity.z = (farMouse.z - mBeforeFar.z) / cameraFar * fLength;
		break;

	}
	objPosition += velocity;
	obj->SetPosition(objPosition);
}

void StageObjDragOperation::SetArrow(std::shared_ptr<StageObj>obj)
{
	VECTOR3F position = obj->GetPosition();
	VECTOR3F scale = obj->GetScale();
	VECTOR3F s[] =
	{
		{VECTOR3F(1,0,0)},
		{VECTOR3F(1,1,0)},
		{VECTOR3F(0,0,1)},
	};
	for (int i = 0;i < 3;i++)
	{
		mArrowObjs[i]->SetScale(VECTOR3F(0.15,0.3,0.15)+ mLocalScale[i]/* + mLocalPosition[i] * mDragObjSize * scale*/);
		mArrowObjs[i]->SetPosition(position + mLocalPosition[i] * mArrowSize.y*(mArrowObjs[i]->GetScale().y));
		mArrowObjs[i]->CalculateTransform();
		mArrowObjs[i]->SetColor(VECTOR4F(s[i].x, s[i].y, s[i].z, 0.5));
	}

	//for (int i = 0;i < 3;i++)
	//{
	//	mArrowObjs[i]->SetPosition(position + localPosition[i]);
	//	
	//	mArrowObjs[i]->CalculateTransform();
	//}
}

void StageObjDragOperation::Render(ID3D11DeviceContext* context, MeshRender* render, const FLOAT4X4& view, const FLOAT4X4& projection)
{
	VECTOR3F light;
	DirectX::XMStoreFloat3(&light, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&(pCamera.GetCamera()->GetFocus() - pCamera.GetCamera()->GetEye()))));
	
	render->Begin(context, view, projection);
	render->Render(context, mArrowObjs[0]->GetMesh(), mArrowObjs[0]->GetWorld(),mArrowObjs[0]->GetColor());
	render->Render(context, mArrowObjs[1]->GetMesh(), mArrowObjs[1]->GetWorld(),mArrowObjs[1]->GetColor());
	render->Render(context, mArrowObjs[2]->GetMesh(), mArrowObjs[2]->GetWorld(),mArrowObjs[2]->GetColor());
	render->End(context);
}
