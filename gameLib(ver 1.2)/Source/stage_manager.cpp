#include "stage_manager.h"
#include"framework.h"
#include"camera_manager.h"
#include"key_board.h"
#include"gpu_particle_manager.h"
/***********************初期化*************************/
StageManager::StageManager(ID3D11Device* device, int width, int height):stageNo(3),mWidth(width), mHeight(height), dragObjNumber(-1)
{
	mMeshs.push_back(std::make_shared<StaticMesh>(device, "Data/FBX/000_cube.fbx"));
	mMeshs.push_back(std::make_shared<StaticMesh>(device, "Data/FBX/jumpstand.fbx"));
	mMeshs.push_back(std::make_shared<StaticMesh>(device, "Data/FBX/go-ru.fbx"));
	mDragOperation = std::make_shared<StageObjDragOperation>(device);
	mRender = std::make_unique<MeshRender>(device);
	{
		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.ByteWidth = sizeof(FLOAT4X4);
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		HRESULT hr = device->CreateBuffer(&bufferDesc, nullptr, mCbBeforeBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	mVelocityShader = std::make_unique<DrowShader>(device, "Data/shader/static_mesh_blur_vs.cso", "", "Data/shader/static_mesh_blur_ps.cso", inputElementDesc,ARRAYSIZE(inputElementDesc));
	Load();
}
/*********************ファイル操作*************************/
//ロード
void StageManager::Load()
{
	FILE* fp;
	std::string fileName = { "Data/file/stage" };
	fileName += std::to_string(stageNo) + ".bin";
	std::vector<StageData>data;
	int size = 0;
	if (fopen_s(&fp, fileName.c_str(), "rb") == 0)
	{
		fread(&size, sizeof(int), 1, fp);
		if (size > 0)
		{
			data.resize(size);
			fread(&data[0], sizeof(StageData), size, fp);
			for (auto& d : data)
			{
				mStageObjs.push_back(std::make_shared<StageObj>(mMeshs[d.mObjType]));
				mStageObjs.back()->SetStageData(d);
			}
		}
		fclose(fp);
		pGpuParticleManager.CreateStageObjParticle(mStageObjs);
	}
}
//セーブ
void StageManager::Save()
{
	FILE* fp;
	std::string fileName = { "Data/file/stage" };
	fileName += std::to_string(stageNo) + ".bin";
	std::vector<StageData>data;
	int size = mStageObjs.size();
	fopen_s(&fp, fileName.c_str(), "wb");
	fwrite(&size, sizeof(int), 1, fp);
	if (size > 0)
	{
		for (auto& obj : mStageObjs)
		{
			data.push_back(obj->GetStageData());
		}
		fwrite(&data[0], sizeof(StageData), size, fp);
	}
	fclose(fp);
}
/*******************エディター(簡易)**********************/
void StageManager::ImGuiUpdate()
{
#ifdef USE_IMGUI
	MouseToWorld();
	if (dragObjNumber == -1)
	{
		if (pKeyBoad.RisingState(VK_LBUTTON))
		{
			dragObjNumber = CheckMouseDragObj();
			if(dragObjNumber!=-1)mDragOperation->DragStart(mFarMouse, mStageObjs[dragObjNumber]->GetMesh()->GetMinPosition(), mStageObjs[dragObjNumber]->GetMesh()->GetMaxPosition());
		}
	}
	ImGui::Begin("stage editer");
	int num = stageNo;
	for (int i = 0;i < 5;i++)
	{
		std::string name = "sateg";
		name += std::to_string(i);
		ImGui::RadioButton(name.c_str(), &stageNo, i);
		if (i < 4)ImGui::SameLine();
	}
	if (num != stageNo)
	{
		mStageObjs.clear();
		Load();
	}
	if (ImGui::CollapsingHeader("create stage obj"))
	{
		static int objNo = 0;
		static const char* StageObjNames[] =
		{
			"floor","gimmick","goal"
		};
		ImGui::Combo("stage obj", &objNo, StageObjNames, 3);
		if (ImGui::Button("create"))
		{
			dragObjNumber = static_cast<int>(mStageObjs.size());
			mStageObjs.push_back(std::make_shared<StageObj>(mMeshs[objNo]));
		}
	}
	ImGui::Text("dragNumber%d", dragObjNumber);
	ImGui::End();
	//stageobjのドラッグ
	if (dragObjNumber != -1)
	{
		dragObjNumber = mDragOperation->Update(mStageObjs[dragObjNumber], mNearMouse, mFarMouse, dragObjNumber);
	}
#endif
}
/******************マウスでドラッグするオブジェクト判定*********************/
int StageManager::CheckMouseDragObj()
{
	int objNumber = -1;
	float minLength = 10000;
	VECTOR3F position, normal;
	for (int i = 0;i < mStageObjs.size();i++)
	{
		float length = 0;
		if (mStageObjs[i]->RayPick(mNearMouse,mFarMouse,&position,&normal, &length) != -1)
		{
			if (minLength > length)
			{
				objNumber = i;
				minLength = length;
			}
		}
	}
	return objNumber;
}
/******************更新**********************/
void StageManager::Update(float elapsd_time)
{
	ImGuiUpdate();
	for (auto& stage : mStageObjs)
	{
		stage->CalculateTransform();
	}
}
/***************描画******************/
void StageManager::Render(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection, const int stageState)
{
	mRender->Begin(context, view, projection);
	for (auto& stage : mStageObjs)
	{
		int state = stage->GetStageData().mColorType + stageState;
		if (state % 2 == 1)continue;
		mRender->Render(context, stage->GetMesh(), stage->GetWorld(),stage->GetColor());
	}
	mRender->End(context);

	//pGpuParticleManager.Render(context, view, projection);

	mRender->Begin(context, view, projection);

	for (auto& stage : mStageObjs)
	{
		int state = stage->GetStageData().mColorType + stageState;
		if (state % 2 == 0)continue;
		mRender->Render(context, stage->GetMesh(), stage->GetWorld(), stage->GetColor());
	}
	mRender->End(context);
	//mDragOperation->Render(context, mRender.get(),view,projection);
}
void StageManager::RenderVelocity(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection, const int stageState)
{
	mRender->Begin(context, view, projection);
	for (auto& stage : mStageObjs)
	{
		int state = stage->GetStageData().mColorType + stageState;
		if (state % 2 == 1)continue;
		context->VSSetConstantBuffers(2, 1, mCbBeforeBuffer.GetAddressOf());
		context->PSSetConstantBuffers(2, 1, mCbBeforeBuffer.GetAddressOf());
		context->UpdateSubresource(mCbBeforeBuffer.Get(), 0, 0, &stage->GetBeforeWorld(), 0, 0);
		mRender->Render(context, mVelocityShader.get(), stage->GetMesh(), stage->GetWorld(), stage->GetColor());
		stage->SetBeforeWorld(stage->GetWorld());
	}
	mRender->End(context);

	//pGpuParticleManager.Render(context, view, projection);

	mRender->Begin(context, view, projection);

	for (auto& stage : mStageObjs)
	{
		int state = stage->GetStageData().mColorType + stageState;
		if (state % 2 == 0)continue;
		context->VSSetConstantBuffers(2, 1, mCbBeforeBuffer.GetAddressOf());
		context->PSSetConstantBuffers(2, 1, mCbBeforeBuffer.GetAddressOf());
		context->UpdateSubresource(mCbBeforeBuffer.Get(), 0, 0, &stage->GetBeforeWorld(), 0, 0);
		mRender->Render(context, mVelocityShader.get(), stage->GetMesh(), stage->GetWorld(), stage->GetColor());
		stage->SetBeforeWorld(stage->GetWorld());
	}
	mRender->End(context);

}
/*******************マウス座標系からワールド座標系に変換**********************/
void StageManager::ScreeenToWorld(VECTOR3F* worldPosition, const VECTOR3F& screenPosition)
{
	float viewportX = 0.0f;
	float viewportY = 0.0f;
	float viewportW = static_cast<float>(mWidth);
	float viewportH = static_cast<float>(mHeight);
	float viewportMinZ = 0.0f;
	float viewportMaxZ = 1.0f;
	// ビュー行列
	DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&pCamera.GetCamera()->GetView());
	// プロジェクション行列
	DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&pCamera.GetCamera()->GetProjection());
	// ワールド行列
	DirectX::XMMATRIX W = DirectX::XMMatrixIdentity();//移動成分はいらないので単位行列を入れておく。
	// スクリーン座標からNDC座標へ変換
	DirectX::XMVECTOR NDCPos = DirectX::XMVectorSet(
		2.0f * (screenPosition.x - viewportX) / viewportW - 1.0f,
		1.0f - 2.0f * (screenPosition.y - viewportY) / viewportH,
		(screenPosition.z - viewportMinZ) / (viewportMaxZ - viewportMinZ),
		1.0f);
	// NDC座標からワールド座標へ変換
	DirectX::XMMATRIX WVP = W * V * P;
	DirectX::XMMATRIX IWVP = DirectX::XMMatrixInverse(nullptr, WVP);
	DirectX::XMVECTOR WPos = DirectX::XMVector3TransformCoord(NDCPos, IWVP);
	DirectX::XMStoreFloat3(worldPosition, WPos);

}

void StageManager::MouseToWorld()
{
	POINT cursor;
	GetCursorPos(&cursor);
	ScreenToClient(Framework::Instance().GetHwnd(), &cursor);
	VECTOR3F positionNear = VECTOR3F(static_cast<float>(cursor.x), static_cast<float>(cursor.y), 0);
	VECTOR3F positionFar = VECTOR3F(static_cast<float>(cursor.x), static_cast<float>(cursor.y), 1);

	ScreeenToWorld(&mNearMouse, positionNear);
	ScreeenToWorld(&mFarMouse, positionFar);

}
