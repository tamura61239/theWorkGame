#include "stage_obj_particle.h"
#include"misc.h"
#include"shader.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif
StageObjParticle::StageObjParticle(ID3D11Device* device) :redObjSize(0), blueObjSize(0), particleSize(0), particleOneObjSize(100)
{
	HRESULT hr;
	//定数バッファ作成
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.ByteWidth = sizeof(CbBuffer);
		desc.StructureByteStride = 0;
		hr = device->CreateBuffer(&desc, nullptr, mCbBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		desc.ByteWidth = sizeof(CbStartData);
		hr = device->CreateBuffer(&desc, nullptr, mCbStartData.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	//シェーダー作成
	{
		hr = create_cs_from_cso(device, "Data/shader/stage_obj_particle_cs.cso", mCSShader.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		hr = create_cs_from_cso(device, "Data/shader/stage_obj_particle_create_cs.cso", mCSCreateShader.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		//hr = create_gs_from_cso(device, "Data/shader/run_particle_gs.cso", mGSShader.GetAddressOf());
		hr = create_gs_from_cso(device, "Data/shader/stage_obj_particle_gs.cso", mGSShader.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		hr = create_ps_from_cso(device, "Data/shader/stage_obj_particle_ps.cso", mPSShader.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
		{
			{"POSITION",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"LIFE",0,DXGI_FORMAT_R32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"SCALE",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"VELOCITY",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"COLORTYPE",0,DXGI_FORMAT_R32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"ANGLE",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		};
		hr = create_vs_from_cso(device, "Data/shader/stage_obj_particle_vs.cso", mVSShader.GetAddressOf(), mInput.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	LoadCbData();
}

void StageObjParticle::CreateBuffer(ID3D11Device* device, std::vector<std::shared_ptr<StageObj>> stages)
{

	//std::vector<Obj>redObjs;
	//std::vector<Obj>blueObjs;
	std::vector<Obj>objs;

	for (int i = 0; i < static_cast<int>(stages.size()); i++)
	{
		auto& stage = stages[i];
		if (stage->GetStageData().mObjType > 0)continue;
		Obj obj;
		VECTOR3F position = stage->GetPosition();
		VECTOR3F scale = stage->GetScale();

		obj.max = position + scale;
		obj.min = position - scale;

		if (stage->GetStageData().mColorType == 0)
		{
			redObjSize++;
			objs.push_back(obj);
		}
	}

	for (int i = 0; i < static_cast<int>(stages.size()); i++)
	{
		auto& stage = stages[i];
		if (stage->GetStageData().mObjType > 0)continue;
		Obj obj;
		VECTOR3F position = stage->GetPosition();
		VECTOR3F scale = stage->GetScale();

		obj.max = position + scale;
		obj.min = position - scale;

		if (stage->GetStageData().mColorType == 1)
		{
			objs.push_back(obj);
		}
	}
	objSize = objs.size();
	//redObjSize = redObjs.size();
	//blueObjSize = blueObjs.size();

	std::vector<Particle>particles;
	particles.resize((objs.size()) * particleOneObjSize);
	particleSize = particles.size();
	
	std::vector<VECTOR3F>datas;
	SetRandBufferData(datas);
	HRESULT hr;
	//バッファ生成
	{
		D3D11_BUFFER_DESC desc;
		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&desc, sizeof(desc));
		ZeroMemory(&data, sizeof(data));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
		//ステージのオブジェクト
		desc.ByteWidth = sizeof(Obj) * objs.size();
		data.pSysMem = &objs[0];
		hr = device->CreateBuffer(&desc, &data, mStageObjs.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		//乱数バッファ
		desc.ByteWidth = sizeof(VECTOR3F) * datas.size();
		data.pSysMem = &datas[0];
		hr = device->CreateBuffer(&desc, &data, mRands.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		//パーティクル
		ZeroMemory(&data, sizeof(data));
		desc.ByteWidth = sizeof(Particle) * particleSize;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS;
		data.pSysMem = &particles[0];
		hr = device->CreateBuffer(&desc, &data, mParticleBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}
	//UAV生成
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement = 0;
		desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
		//パーティクル
		desc.Buffer.NumElements = sizeof(Particle) * particleSize / 4;
		hr = device->CreateUnorderedAccessView(mParticleBuffer.Get(), &desc, mParticleUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	//SRV
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
		desc.BufferEx.FirstElement = 0;
		desc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
		//乱数バッファのSRV
		desc.BufferEx.NumElements = sizeof(VECTOR3F) * datas.size() / 4;
		hr = device->CreateShaderResourceView(mRands.Get(), &desc, mRandsSRV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		//ステージオブジェクトのSRV
		desc.BufferEx.NumElements = sizeof(Obj) * objs.size() / 4;
		hr = device->CreateShaderResourceView(mStageObjs.Get(), &desc, mStageObjsSRV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	mStartData.changeColorFlag = 0;
	mCb.elapsdTime = 0;
	mStartData.startIndex = 0;
	mStartData.redNumber = static_cast<float>(redObjSize);
	mCb.nowColorType = 0;
	mCb.maxSize = static_cast<float>(objs.size());
	beforeIndex = -1;
}

void StageObjParticle::ImGuiUpdate(float elapsdTime)
{
#ifdef USE_IMGUI
	ImGui::Begin("stage obj particle");
	{
		float* angleMovement[3] = { &mCb.angleMovement.x,&mCb.angleMovement.y,&mCb.angleMovement.z };
		ImGui::SliderFloat3("angleMovement", *angleMovement, -DirectX::XMConvertToRadians(360), DirectX::XMConvertToRadians(360));
		float* blueColor[4] = { &mStartData.blueColor.x,&mStartData.blueColor.y ,&mStartData.blueColor.z ,&mStartData.blueColor.w };
		ImGui::ColorEdit4("blue color", *blueColor);
		float* redColor[4] = { &mStartData.redColor.x,&mStartData.redColor.y ,&mStartData.redColor.z ,&mStartData.redColor.w };
		ImGui::ColorEdit4("red color", *redColor);
		ImGui::InputFloat("create particle size", &mStartData.indexSize);
		ImGui::InputFloat("max life", &mCb.maxLife);
		if (ImGui::Button("stage obj particle save"))SaveCbData();
	}
	ImGui::End();
#endif
}

void StageObjParticle::Update(ID3D11DeviceContext* context, float elapsd_time, const int colorState)
{
	mCb.elapsdTime = elapsd_time;
	if (mParticleUAV.Get() == nullptr)return;
	context->CSSetConstantBuffers(0, 1, mCbBuffer.GetAddressOf());
	context->CSSetConstantBuffers(1, 1, mCbStartData.GetAddressOf());
	context->UpdateSubresource(mCbBuffer.Get(), 0, 0, &mCb, 0, 0);
	context->UpdateSubresource(mCbStartData.Get(), 0, 0, &mStartData, 0, 0);

	ID3D11UnorderedAccessView* uav[1] =
	{
		mParticleUAV.Get(),
	};
	ID3D11ShaderResourceView* srv[2] =
	{
		mRandsSRV.Get(),
		mStageObjsSRV.Get(),
	};
	context->CSSetShaderResources(0, 2, srv);
	context->CSSetUnorderedAccessViews(0, 1, uav, nullptr);


	if (mCb.nowColorType != colorState)mStartData.changeColorFlag = 1;
	else mStartData.changeColorFlag = 0;
	mCb.nowColorType = static_cast<float>(colorState);
	mStartData.startIndex += mStartData.indexSize * elapsd_time * 0.5f;
	if (mStartData.startIndex - beforeIndex >= 1.0f)
	{
		context->CSSetShader(mCSCreateShader.Get(), nullptr, 0);
		context->Dispatch(objSize, 1, 1);
		beforeIndex++;
		if (beforeIndex >= particleOneObjSize)beforeIndex -= particleOneObjSize;
	}
	if (mStartData.startIndex >= particleOneObjSize)mStartData.startIndex -= particleOneObjSize;

	context->CSSetShader(mCSShader.Get(), nullptr, 0);
	context->Dispatch(particleSize / 100, 1, 1);

	srv[0] = nullptr;
	srv[1] = nullptr;
	context->CSSetShaderResources(0, 2, srv);

	uav[0] = nullptr;
	context->CSSetUnorderedAccessViews(0, 1, uav, nullptr);

	context->CSSetShader(nullptr, nullptr, 0);
}

void StageObjParticle::Render(ID3D11DeviceContext* context)
{
	if (mParticleUAV.Get() == nullptr)return;

	context->VSSetShader(mVSShader.Get(), nullptr, 0);
	context->GSSetShader(mGSShader.Get(), nullptr, 0);
	context->PSSetShader(mPSShader.Get(), nullptr, 0);
	context->IASetInputLayout(mInput.Get());
	u_int stride = sizeof(Particle);
	u_int offset = 0;

	context->IASetVertexBuffers(0, 1, mParticleBuffer.GetAddressOf(), &stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	context->Draw(particleSize, 0);

	context->VSSetShader(nullptr, nullptr, 0);
	context->GSSetShader(nullptr, nullptr, 0);
	context->PSSetShader(nullptr, nullptr, 0);
}

void StageObjParticle::SetRandBufferData(std::vector<VECTOR3F>& data)
{
	FILE* fp;
	if (fopen_s(&fp, "Data/file/stage_obj_particle_random.bin", "rb") == 0)
	{
		data.resize(400);
		fread(&data[0], sizeof(VECTOR3F), 400, fp);
		fclose(fp);
		return;
	}
	for (int i = 0; i < 400; i++)
	{
		data.push_back(VECTOR3F(static_cast<float>(rand()) / 32767.f, static_cast<float>(rand()) / 32767.f, static_cast<float>(rand()) / 32767.f));
	}
	fopen_s(&fp, "Data/file/stage_obj_particle_random.bin", "wb");
	fwrite(&data[0], sizeof(VECTOR3F), 400, fp);
	fclose(fp);
}

void StageObjParticle::LoadCbData()
{
	FILE* fp;
	if (fopen_s(&fp, "Data/file/stage_obj_particle_data.bin", "rb") == 0)
	{
		fread(&mStartData.redColor, sizeof(VECTOR4F), 1, fp);
		fread(&mStartData.blueColor,sizeof(VECTOR4F),1, fp);
		fread(&mCb.maxLife, sizeof(float), 1, fp);
		fread(&mStartData.indexSize, sizeof(float), 1, fp);
		fread(&mCb.angleMovement, sizeof(VECTOR3F), 1, fp);
		fclose(fp);
		return;
	}
	mStartData.blueColor = VECTOR4F(0, 0, 1, 1);
	mStartData.redColor = VECTOR4F(1, 0, 0, 1);
	mCb.angleMovement = VECTOR3F(DirectX::XMConvertToRadians(200), DirectX::XMConvertToRadians(100), 0);
	mStartData.indexSize = 10;
	mCb.maxLife = 1.0f;

}

void StageObjParticle::SaveCbData()
{
	FILE* fp;
	fopen_s(&fp, "Data/file/stage_obj_particle_data.bin", "wb");
	fwrite(&mStartData.redColor, sizeof(VECTOR4F), 1, fp);
	fwrite(&mStartData.blueColor, sizeof(VECTOR4F), 1, fp);
	fwrite(&mCb.maxLife, sizeof(float), 1, fp);
	fwrite(&mStartData.indexSize, sizeof(float), 1, fp);
	fwrite(&mCb.angleMovement, sizeof(VECTOR3F), 1, fp);
	fclose(fp);

}
