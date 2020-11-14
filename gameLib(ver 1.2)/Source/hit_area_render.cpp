#include "hit_area_render.h"


void HitAreaRender::Initialize(ID3D11Device* device)
{
	mMesh = std::make_unique<StaticMesh>(device, "Data/FBX/000_cube.fbx");
	mRender = std::make_unique<MeshRender>(device);
	mCount = 0;
}

void HitAreaRender::SetObjSize(const int size)
{
	for (int i = 0; i < size; i++)
	{
		mObjData.push_back(std::make_unique<Obj3D>());
	}
}

void HitAreaRender::SetObjData(const VECTOR3F& position, const VECTOR3F& scale)
{
	if (mCount >= mObjData.size())
	{
		mObjData.push_back(std::make_unique<Obj3D>());
	}
	mObjData.at(mCount)->SetPosition(position);
	mObjData.at(mCount)->SetScale(scale);
	mObjData.at(mCount)->CalculateTransform();
	mCount++;
}


void HitAreaRender::Render(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection)
{
	mRender->Begin(context, view, projection, true);
	for (auto& obj : mObjData)
	{
		mRender->Render(context, mMesh.get(), obj->GetWorld());
	}
	mRender->End(context);
}

void HitAreaRender::ClearData()
{
	mObjData.clear();
	mCount = 0;
}
