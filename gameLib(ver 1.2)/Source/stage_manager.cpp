#include "stage_manager.h"
#include"framework.h"
#include"camera_manager.h"
#include"key_board.h"
#include"gpu_particle_manager.h"
#include"hit_area_render.h"
int StageManager::mMaxStage = 0;
/***********************初期化*************************/
StageManager::StageManager(ID3D11Device* device, int width, int height) :mStageNo(3), mWidth(static_cast<float>(width)), mHeight(static_cast<float>(height)), dragObjNumber(-1)
{
	mMeshs.push_back(std::make_shared<StaticMesh>(device, "Data/FBX/000_cube.fbx"));
	mMeshs.push_back(std::make_shared<StaticMesh>(device, "Data/FBX/jumpstand.fbx"));
	mMeshs.push_back(std::make_shared<StaticMesh>(device, "Data/FBX/go-ru.fbx"));
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

	mEditor = std::make_unique<StageEditor>(device, 1920, 1080);
}
void StageManager::StageCount()
{
	FILE* fp;
	mMaxStage = 0;
	while (1)
	{
		std::string fileName = { "Data/file/stage" };
		fileName += std::to_string(mMaxStage) + ".bin";
		if (fopen_s(&fp, fileName.c_str(), "rb") == 0)
		{
			fclose(fp);
			mMaxStage++;
		}
		else
		{
			break;
		}
	}

}
/*********************ファイル操作*************************/
//ロード
void StageManager::Load()
{
	FILE* fp;
	std::string fileName = { "Data/file/stage" };
	fileName += std::to_string(mStageNo) + ".bin";
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
	}
}
//セーブ
void StageManager::Save()
{
	FILE* fp;
	std::string fileName = { "Data/file/stage" };
	fileName += std::to_string(mStageNo) + ".bin";
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
	mEditor->Update(mStageObjs);
	switch (mEditor->GetFileState())
	{
	case 1:
		Clear();
		Load();
		break;
	case 2:
		Save();
		break;
	}
	mEditor->ClearFileState();
	if (mEditor->GetCreateFlag())
	{
		StageData data = mEditor->GetCreateData();
		mStageObjs.push_back(std::make_shared<StageObj>(mMeshs[data.mObjType]));
		mStageObjs.back()->SetStageData(data);
		mEditor->ClearCreateData();
	}
	int deleteNo = mEditor->GetDeleteNo();
	if (deleteNo != -1)
	{
		mStageObjs.erase(mStageObjs.begin() + deleteNo);
	}
#endif
}
/******************更新**********************/
void StageManager::Update(float elapsd_time)
{
	for (auto& stage : mStageObjs)
	{
		stage->CalculateTransform();
		switch (stage->GetStageData().mObjType)
		{
		case 0:
			HitAreaRender::GetInctance()->SetObjData(stage->GetPosition(), stage->GetScale());
			break;
		case 1:
			HitAreaRender::GetInctance()->SetObjData(stage->GetPosition() + VECTOR3F(0, stage->GetScale().y, 0), stage->GetScale());
			break;
		case 2:
			HitAreaRender::GetInctance()->SetObjData(stage->GetPosition() + VECTOR3F(0, stage->GetScale().y * 3, 0), stage->GetScale() * VECTOR3F(1.5f, 3, 0.5f));
			break;
		}
	}
}
/***************描画******************/
void StageManager::Render(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection, const int stageState, DrowShader* srv)
{
	VECTOR4F color[2];
	D3D11_VIEWPORT viewport;
	UINT num_viewports = 1;
	context->RSGetViewports(&num_viewports, &viewport);
	VECTOR3F viewNear[4], viewFar[4];
	float x1 = static_cast<float>(viewport.TopLeftX)-150.f;
	float y1 = static_cast<float>(viewport.TopLeftY) - 150.f;
	float x2 = x1 + static_cast<float>(viewport.Width) + 300.f;
	float y2 = y1 + static_cast<float>(viewport.Height) + 300.f;
	viewNear[0] = VECTOR3F(x1, y1, 0);
	viewNear[1] = VECTOR3F(x2, y1, 0);
	viewNear[2] = VECTOR3F(x1, y2, 0);
	viewNear[3] = VECTOR3F(x2, y2, 0);
	viewFar[0] = VECTOR3F(x1, y1, 1);
	viewFar[1] = VECTOR3F(x2, y1, 1);
	viewFar[2] = VECTOR3F(x1, y2, 1);
	viewFar[3] = VECTOR3F(x2, y2, 1);
	if (srv == nullptr)
	{
		mRender->Begin(context, view, projection);
		for (auto& stage : mStageObjs)
		{
			int state = stage->GetStageData().mColorType + stageState;
			if (state % 2 == 1)continue;

			color[0] = stage->GetColor();
			if (!FrustumCulling(stage.get(), view, projection, viewNear, viewFar, static_cast<float>(viewport.Height), static_cast<float>(viewport.Width)))continue;
			mRender->Render(context, stage->GetMesh(), stage->GetWorld(), stage->GetColor());
		}
		for (auto& stage : mStageObjs)
		{
			int state = stage->GetStageData().mColorType + stageState;
			if (state % 2 == 0)continue;
			color[1] = stage->GetColor();
			if (!FrustumCulling(stage.get(), view, projection, viewNear, viewFar, static_cast<float>(viewport.Height), static_cast<float>(viewport.Width)))continue;

			mRender->Render(context, stage->GetMesh(), stage->GetWorld(), stage->GetColor());
		}
		mEditor->EditorCreateObjImageRender(context, mMeshs.at(mEditor->GetCreateData().mObjType).get(), mRender.get(), color[mEditor->GetCreateData().mColorType]);
		mRender->End(context);
	}
	else
	{
		mRender->Begin(context, view, projection);
		for (auto& stage : mStageObjs)
		{
			int state = stage->GetStageData().mColorType + stageState;
			if (state % 2 == 1)continue;
			if (!FrustumCulling(stage.get(), view, projection, viewNear, viewFar, static_cast<float>(viewport.Height), static_cast<float>(viewport.Width)))continue;

			mRender->Render(context, srv, stage->GetMesh(), stage->GetWorld(), stage->GetColor());
		}
		for (auto& stage : mStageObjs)
		{
			int state = stage->GetStageData().mColorType + stageState;
			if (state % 2 == 0)continue;
			if (!FrustumCulling(stage.get(), view, projection, viewNear, viewFar, static_cast<float>(viewport.Height), static_cast<float>(viewport.Width)))continue;
			mRender->Render(context, srv, stage->GetMesh(), stage->GetWorld(), stage->GetColor());
		}
		mRender->End(context);

	}
}
void StageManager::RenderVelocity(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection, const int stageState)
{
	D3D11_VIEWPORT viewport;
	UINT num_viewports = 1;
	context->RSGetViewports(&num_viewports, &viewport);
	VECTOR3F viewNear[4], viewFar[4];
	float x1 = static_cast<float>(viewport.TopLeftX) - 150.f;
	float y1 = static_cast<float>(viewport.TopLeftY) - 150.f;
	float x2 = x1 + static_cast<float>(viewport.Width) + 300.f;
	float y2 = y1 + static_cast<float>(viewport.Height) + 300.f;
	viewNear[0] = VECTOR3F(x1, y1, 0);
	viewNear[1] = VECTOR3F(x2, y1, 0);
	viewNear[2] = VECTOR3F(x1, y2, 0);
	viewNear[3] = VECTOR3F(x2, y2, 0);
	viewFar[0] = VECTOR3F(x1, y1, 1);
	viewFar[1] = VECTOR3F(x2, y1, 1);
	viewFar[2] = VECTOR3F(x1, y2, 1);
	viewFar[3] = VECTOR3F(x2, y2, 1);

	mRender->VelocityBegin(context, view, projection);
	for (auto& stage : mStageObjs)
	{
		if (!FrustumCulling(stage.get(), view, projection, viewNear, viewFar, static_cast<float>(viewport.Height), static_cast<float>(viewport.Width)))continue;
		mRender->VelocityRender(context, stage->GetMesh(), stage->GetWorld(), stage->GetBeforeWorld(), stage->GetColor());
		stage->SetBeforeWorld(stage->GetWorld());
	}
	mRender->VelocityEnd(context);

}
void StageManager::RenderShadow(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection)
{
	D3D11_VIEWPORT viewport;
	UINT num_viewports = 1;
	context->RSGetViewports(&num_viewports, &viewport);
	VECTOR3F viewNear[4], viewFar[4];
	float x1 = static_cast<float>(viewport.TopLeftX) - 150.f;
	float y1 = static_cast<float>(viewport.TopLeftY) - 150.f;
	float x2 = x1 + static_cast<float>(viewport.Width) + 300.f;
	float y2 = y1 + static_cast<float>(viewport.Height) + 300.f;
	viewNear[0] = VECTOR3F(x1, y1, 0);
	viewNear[1] = VECTOR3F(x2, y1, 0);
	viewNear[2] = VECTOR3F(x1, y2, 0);
	viewNear[3] = VECTOR3F(x2, y2, 0);
	viewFar[0] = VECTOR3F(x1, y1, 1);
	viewFar[1] = VECTOR3F(x2, y1, 1);
	viewFar[2] = VECTOR3F(x1, y2, 1);
	viewFar[3] = VECTOR3F(x2, y2, 1);

	mRender->ShadowBegin(context, view, projection);
	for (auto& stage : mStageObjs)
	{
		if (!FrustumCulling(stage.get(), view, projection, viewNear, viewFar, static_cast<float>(viewport.Height), static_cast<float>(viewport.Width)))continue;
		mRender->ShadowRender(context, stage->GetMesh(), stage->GetWorld(), stage->GetColor());
		stage->SetBeforeWorld(stage->GetWorld());
	}
	mRender->ShadowEnd(context);

}
void StageManager::SidoViewRender(ID3D11DeviceContext* context)
{
	if (!mEditor->GetEditorFlag())return;
	FrameBuffer* frame = mEditor->GetStageSidoViewBuffer();
	frame->Clear(context, 0.5f, 0.5f, 0.5f, 1);
	frame->Activate(context);
	mRender->Begin(context, mEditor->GetCamera()->GetView(), mEditor->GetCamera()->GetProjection());
	for (auto& stage : mStageObjs)
	{
		int state = stage->GetStageData().mColorType;
		if (state % 2 == 1)continue;
		mRender->Render(context, stage->GetMesh(), stage->GetWorld(), stage->GetColor());
	}

	for (auto& stage : mStageObjs)
	{
		int state = stage->GetStageData().mColorType;
		if (state % 2 == 0)continue;
		mRender->Render(context, stage->GetMesh(), stage->GetWorld(), stage->GetColor());
	}
	frame->Deactivate(context);
	mEditor->SidoViewRender(context);
}

void StageManager::ScreenToWorld(VECTOR3F* worldPosition, const VECTOR3F& screenPosition, const FLOAT4X4& view, const FLOAT4X4& projection, const float height, const float width)
{
	float viewportX = 0.0f;
	float viewportY = 0.0f;
	float viewportW = static_cast<float>(width);
	float viewportH = static_cast<float>(height);
	float viewportMinZ = 0.0f;
	float viewportMaxZ = 1.0f;

	// ビュー行列
	DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&view);
	// プロジェクション行列
	DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&projection);
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

bool StageManager::FrustumCulling(StageObj* obj, const FLOAT4X4& view, const FLOAT4X4& projection, const VECTOR3F* nears, const VECTOR3F* fars, const float height, const float width)
{
	VECTOR3F  worldCenter, nearCenter=VECTOR3F(0,0,0), farCenter = VECTOR3F(0, 0, 0);

	VECTOR3F worldNear[4], worldFar[4];
	for (int i = 0; i < 4; i++)
	{
		ScreenToWorld(&worldNear[i], nears[i], view, projection, height, width);
		ScreenToWorld(&worldFar[i], fars[i], view, projection, height, width);
		nearCenter += worldNear[i];
		farCenter += worldFar[i];
	}
	nearCenter *= 0.25f;
	farCenter *= 0.25f;

	worldCenter = farCenter + nearCenter;

	worldCenter *= 0.5f;

	VECTOR3F position = obj->GetPosition();
	VECTOR3F scale = obj->GetScale();
	VECTOR3F cube[8] =
	{
		VECTOR3F(-1,-1,-1),
		VECTOR3F(-1,1,-1),
		VECTOR3F(1,-1,-1),
		VECTOR3F(1,1,-1),
		VECTOR3F(-1,-1,1),
		VECTOR3F(-1,1,1),
		VECTOR3F(1,-1,1),
		VECTOR3F(1,1,1),
	};
	VECTOR3F scale2;
	switch (obj->GetStageData().mObjType)
	{
	case 0:
		scale2 = VECTOR3F(1, 1, 1);
		break;
	case 1:
		scale2 = VECTOR3F(2, 2, 2);
		break;
	case 2:
		scale2 = VECTOR3F(1.5f, 3, 0.5f);
		break;
	}
	float minLength = FLT_MAX;
	int num = -1;
	VECTOR3F minPosition;
	for (auto& point : cube)
	{
		float length = 0;
		VECTOR3F pos = position + point * scale * scale2;
		DirectX::XMVECTOR vPos = DirectX::XMLoadFloat3(&pos);
		DirectX::XMVECTOR vCenter = DirectX::XMLoadFloat3(&worldCenter);
		DirectX::XMVECTOR vLength = DirectX::XMVector3Length(DirectX::XMVectorSubtract(vPos, vCenter));
		DirectX::XMStoreFloat(&length, vLength);
		length = abs(length);
		if (minLength > length)
		{
			minLength = length;
			minPosition = pos;
		}
	}

	struct Mesh
	{
		VECTOR3F point[4];
		VECTOR3F normal;
	};
	std::vector<Mesh>meshs;
	meshs.resize(4);
	//up
	meshs[0].point[0] = worldNear[0];
	meshs[0].point[1] = worldNear[1];
	meshs[0].point[2] = worldFar[0];
	meshs[0].point[3] = worldFar[1];
	//down
	meshs[1].point[0] = worldNear[3];
	meshs[1].point[1] = worldNear[2];
	meshs[1].point[2] = worldFar[3];
	meshs[1].point[3] = worldFar[2];
	//right
	meshs[2].point[0] = worldNear[3];
	meshs[2].point[1] = worldFar[3];
	meshs[2].point[2] = worldNear[1];
	meshs[2].point[3] = worldFar[1];
	//left
	meshs[3].point[0] = worldNear[0];
	meshs[3].point[1] = worldFar[0];
	meshs[3].point[2] = worldNear[2];
	meshs[3].point[3] = worldFar[2];
	
	for (auto& mesh : meshs)
	{
		DirectX::XMVECTOR position1, position2, position3;
		position1 = DirectX::XMLoadFloat3(&mesh.point[0]);
		position2 = DirectX::XMLoadFloat3(&mesh.point[1]);
		position3 = DirectX::XMLoadFloat3(&mesh.point[2]);
		DirectX::XMVECTOR vec1, vec2;
		vec1 = DirectX::XMVectorSubtract(position2, position1);
		vec2 = DirectX::XMVectorSubtract(position3, position1);
		DirectX::XMVECTOR normal = DirectX::XMVector3Cross(vec1, vec2);
		normal = DirectX::XMVector3Normalize(normal);
		DirectX::XMVECTOR vec3 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&minPosition), position1);
		vec3 = DirectX::XMVector3Normalize(vec3);
		DirectX::XMVECTOR vDot = DirectX::XMVector3Dot(vec3, normal);
		float dot = 0;
		DirectX::XMStoreFloat(&dot, vDot);
		if (dot < 0)return false;
	}
	return true;
}
