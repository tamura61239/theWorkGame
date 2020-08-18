#include "stage_obj_particle.h"
#include"misc.h"
#include"shader.h"
#define CS_TYPE 1
StageObjParticle::StageObjParticle(ID3D11Device* device) :redObjSize(0), blueObjSize(0), particleSize(0)
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
#if CS_TYPE
		hr = create_cs_from_cso(device, "Data/shader/stage_obj_particle_cs.cso", mCSShader.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
#else
		hr = create_cs_from_cso(device, "Data/shader/stage_obj_particle_cs.cso", mCSShader.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
#endif
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

}

void StageObjParticle::CreateBuffer(ID3D11Device* device, std::vector<std::shared_ptr<StageObj>> stages)
{

	std::vector<Obj>redObjs;
	std::vector<Obj>blueObjs;

	for (int i=0;i<stages.size();i++)
	{
		auto& stage = stages[i];
		if (stage->GetStageData().mObjType > 0)continue;
		Obj obj;
		VECTOR3F position = stage->GetPosition();
		VECTOR3F scale = stage->GetScale();

		obj.max = position + scale;
		obj.min = position - scale;

		switch (stage->GetStageData().mColorType)
		{
		case 0:
			redObjs.push_back(obj);
			break;
		case 1:
			blueObjs.push_back(obj);
			break;
		}
	}
	redObjSize = redObjs.size();
	blueObjSize = blueObjs.size();
	int red = redObjSize % 4;
	if (red != 0)
	{
		for (int i = 0; i < 4 - red; i++)
		{
			Obj obj;
			obj.min = VECTOR3F(0, 0, 0);
			obj.max = VECTOR3F(0, 0, 0);
			redObjs.push_back(obj);
		}
		redObjSize = redObjs.size();
	}
	int blue = blueObjSize % 4;
	if (blue != 0)
	{
		for (int i = 0; i < 4 - blue; i++)
		{
			Obj obj;
			obj.min = VECTOR3F(0, 0, 0);
			obj.max = VECTOR3F(0, 0, 0);
			blueObjs.push_back(obj);
		}
		blueObjSize = blueObjs.size();
	}

	std::vector<Particle>particles;
	particles.resize((redObjSize + blueObjSize) * 1000);
	particleSize = particles.size();

	HRESULT hr;
	//バッファ生成
	{
		D3D11_BUFFER_DESC desc;
		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&desc, sizeof(desc));
		ZeroMemory(&data, sizeof(data));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
		//赤色のオブジェクト
		desc.ByteWidth = sizeof(Obj) * redObjSize;
		data.pSysMem = &redObjs[0];
		hr = device->CreateBuffer(&desc, &data, mRedStageObjs.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		//青色のオブジェクト
		ZeroMemory(&data, sizeof(data));
		desc.ByteWidth = sizeof(Obj) * blueObjSize;
		data.pSysMem = &blueObjs[0];
		hr = device->CreateBuffer(&desc, &data, mBlueStageObjs.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		//パーティクル
		ZeroMemory(&data, sizeof(data));
		desc.ByteWidth = sizeof(Particle) * particleSize;
		data.pSysMem = &particles[0];
		hr = device->CreateBuffer(&desc, &data, mParticleBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}
#if CS_TYPE
	//UAV生成
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement = 0;
		desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
		//赤色のオブジェクト
		desc.Buffer.NumElements = sizeof(Obj) * redObjSize / 4;
		hr = device->CreateUnorderedAccessView(mRedStageObjs.Get(), &desc, mRedStageObjsUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		//青色のオブジェクト
		desc.Buffer.NumElements = sizeof(Obj) * blueObjSize / 4;
		hr = device->CreateUnorderedAccessView(mBlueStageObjs.Get(), &desc, mBlueStageObjsUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		//パーティクル
		desc.Buffer.NumElements = sizeof(Particle) * particleSize / 4;
		hr = device->CreateUnorderedAccessView(mParticleBuffer.Get(), &desc, mParticleUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
#else
	//SRV
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
		desc.BufferEx.FirstElement = 0;
		desc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
		//赤色のオブジェクト
		desc.BufferEx.NumElements = sizeof(Obj) * redObjSize / 4;
		hr = device->CreateShaderResourceView(mRedStageObjs.Get(), &desc, mRedStageObjsSRV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		//青色のオブジェクト
		desc.BufferEx.NumElements = sizeof(Obj) * blueObjSize / 4;
		hr = device->CreateShaderResourceView(mBlueStageObjs.Get(), &desc, mBlueStageObjsSRV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}
	//UAV
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
#endif
	mStartData.blueColor = VECTOR4F(0, 0, 1, 1);
	mStartData.redColor = VECTOR4F(1, 0, 0, 1);
	mCb.angleMovement = VECTOR3F(0, 0, 0);
	mStartData.changeColorFlag = 0;
	mCb.elapsdTime = 0;
	mStartData.indexSize = 10;
	mStartData.startIndex = 0;
	mCb.maxLife = 1.0f;
	mStartData.redNumber = redObjSize;
	mCb.nowColorType = 0;
	mCb.maxSize = redObjSize + blueObjSize;
}

void StageObjParticle::Update(ID3D11DeviceContext* context, float elapsd_time, const int colorState)
{
	mCb.elapsdTime = elapsd_time;
	if (mParticleUAV.Get() == nullptr)return;
	//mCb.startIndex += mCb.indexSize;
	//if (mCb.startIndex >= 1000)mCb.startIndex -= 1000;
	context->CSSetConstantBuffers(0, 1, mCbBuffer.GetAddressOf());
	context->CSSetConstantBuffers(1, 1, mCbStartData.GetAddressOf());
	context->UpdateSubresource(mCbBuffer.Get(), 0, 0, &mCb, 0, 0);
	context->UpdateSubresource(mCbStartData.Get(), 0, 0, &mStartData, 0, 0);

	context->CSSetShader(mCSShader.Get(), nullptr, 0);
#if CS_TYPE
	ID3D11UnorderedAccessView* uav[3] =
	{
		mRedStageObjsUAV.Get(),
		mBlueStageObjsUAV.Get(),
		mParticleUAV.Get(),
	};
	context->CSSetUnorderedAccessViews(0, 3, uav, nullptr);

	context->Dispatch(particleSize/100, 1, 1);

	uav[0] = nullptr;
	uav[1] = nullptr;
	uav[2] = nullptr;
	context->CSSetUnorderedAccessViews(0, 3, uav, nullptr);
	mStartData.indexSize = 0;
#else
	ID3D11UnorderedAccessView* uav[1] =
	{
		mParticleUAV.Get(),
	};
	context->CSSetUnorderedAccessViews(0, 1, uav, nullptr);
	ID3D11ShaderResourceView* srv[2] =
	{
		mRedStageObjsSRV.Get(),
		mBlueStageObjsSRV.Get(),
	};
	context->CSSetShaderResources(0, 2, srv);
	context->Dispatch(particleSize / 100, 1, 1);

	uav[0] = nullptr;
	context->CSSetUnorderedAccessViews(0, 1, uav, nullptr);
	srv[0] = nullptr;
	srv[1] = nullptr;
	context->CSSetShaderResources(0, 2, srv);
#endif
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
