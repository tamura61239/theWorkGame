#include "stage_manager.h"
#include"framework.h"
#include"camera_manager.h"
#include"key_board.h"
#include"gpu_particle_manager.h"
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

	mVelocityShader = std::make_unique<DrowShader>(device, "Data/shader/static_mesh_blur_vs.cso", "", "Data/shader/static_mesh_blur_ps.cso", inputElementDesc, ARRAYSIZE(inputElementDesc));
	mDeferredShader = std::make_unique<DrowShader>(device, "Data/shader/static_mesh_vs.cso", "", "Data/shader/deferred_depth_static_mesh_ps.cso", inputElementDesc, ARRAYSIZE(inputElementDesc));
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
		pGpuParticleManager.CreateStageObjParticle(mStageObjs);
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
	}
}
/***************描画******************/
void StageManager::Render(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection, const int stageState, DrowShader* srv)
{
	VECTOR4F color[2];
	if (srv == nullptr)
	{
		mRender->Begin(context, view, projection);
		for (auto& stage : mStageObjs)
		{
			int state = stage->GetStageData().mColorType + stageState;
			if (state % 2 == 1)continue;
			color[0] = stage->GetColor();
			mRender->Render(context, stage->GetMesh(), stage->GetWorld(), stage->GetColor());
		}
		for (auto& stage : mStageObjs)
		{
			int state = stage->GetStageData().mColorType + stageState;
			if (state % 2 == 0)continue;
			color[1] = stage->GetColor();
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
			mRender->Render(context, srv, stage->GetMesh(), stage->GetWorld(), stage->GetColor());
		}
		for (auto& stage : mStageObjs)
		{
			int state = stage->GetStageData().mColorType + stageState;
			if (state % 2 == 0)continue;
			mRender->Render(context, srv, stage->GetMesh(), stage->GetWorld(), stage->GetColor());
		}
		mRender->End(context);

	}
	mRender->Begin(context, view, projection, true);
#ifdef USE_IMGUI
	if (mEditor->GetSceneSaveFlag())return;
	for (auto& stage : mStageObjs)
	{
		if (stage->GetStageData().mObjType ==2)
		{
			Obj3D obj;
			obj.SetAngle(stage->GetAngle());
			obj.SetScale(stage->GetScale()*VECTOR3F(1.5f,3,0.5f));
			obj.SetPosition(stage->GetPosition()+VECTOR3F(0,obj.GetScale().y,0));
			obj.CalculateTransform();
			mRender->Render(context, mMeshs.at(mEditor->GetCreateData().mObjType).get(), obj.GetWorld(), VECTOR4F(1, 1, 1, 1));
			continue;
		}
		mRender->Render(context, mMeshs.at(mEditor->GetCreateData().mObjType).get(), stage->GetWorld(), VECTOR4F(1,1,1,1));
	}
	mRender->End(context);
#endif
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
