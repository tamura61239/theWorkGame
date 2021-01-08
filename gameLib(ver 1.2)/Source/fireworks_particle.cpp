#include "fireworks_particle.h"
#include<string>
#include"misc.h"
#include"shader.h"
#include"vector_combo.h"

#ifdef USE_IMGUI
#include<imgui.h>
#endif

FireworksParticle::FireworksParticle(ID3D11Device* device) :mCreateFlag(false), mMaxParticle(350000)
, mEmitorTimer(0), mMaxEmitorTime(0), mParameterEditFlag(false), mIndex(0), mStartMaxTime(0), mOneRankEmitorCount(0)
, mDefRanking(0), mNowPlayRanking(0), mState(0), mDefStartState(0), mFireworksCount(0)
{
	HRESULT hr;
	Microsoft::WRL::ComPtr<ID3D11Buffer>buffer;

	{
		D3D11_BUFFER_DESC desc;
		std::vector<Particle>particles;
		particles.resize(mMaxParticle);
		ZeroMemory(&desc, sizeof(desc));
		desc.ByteWidth = sizeof(Particle) * mMaxParticle;
		desc.Usage = D3D11_USAGE_DEFAULT;//ステージの入出力はOK。GPUの入出力OK。
		desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; // 構造化バッファ
		desc.StructureByteStride = sizeof(Particle);
		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = &particles[0];
		hr = device->CreateBuffer(&desc, &data, buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		//描画用バッファ
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
		desc.ByteWidth = sizeof(RenderParticle) * mMaxParticle;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags = 0;
		desc.StructureByteStride = 0;
		std::vector<RenderParticle>renderParticles;
		renderParticles.resize(mMaxParticle);
		data.pSysMem = &renderParticles[0];
		hr = device->CreateBuffer(&desc, &data, mRenderBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		//定数バッファ
		desc.ByteWidth = sizeof(CbUpdate);
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		hr = device->CreateBuffer(&desc, nullptr, mCbUpdateBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		desc.ByteWidth = sizeof(CbCreate);

		hr = device->CreateBuffer(&desc, nullptr, mCbCreateBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}
	//UAV
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Buffer.NumElements = mMaxParticle;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;

		hr = device->CreateUnorderedAccessView(buffer.Get(), &desc, mParticleUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement = 0;
		desc.Buffer.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;

		desc.Buffer.NumElements = sizeof(RenderParticle) * mMaxParticle / 4;
		hr = device->CreateUnorderedAccessView(mRenderBuffer.Get(), &desc, mRenderUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}
	CreateCSFromCso(device, "Data/shader/fireworks_particle_create_cs.cso", mCSCreate1Shader.GetAddressOf());
	CreateCSFromCso(device, "Data/shader/fireworks_smoke_particle_create.cso", mCSCreate2Shader.GetAddressOf());
	CreateCSFromCso(device, "Data/shader/fireworks_particle_cs.cso", mCSShader.GetAddressOf());

	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"ANGLE",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"VELOCITY",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"SCALE",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
	};
#if 0
	mShader = std::make_unique<DrowShader>(device, "Data/shader/particle_render_vs.cso", "Data/shader/particle_render_billboard_gs.cso", "Data/shader/particle_render_ps.cso", inputElementDesc, ARRAYSIZE(inputElementDesc));
#else
	mShader = std::make_unique<DrowShader>(device, "Data/shader/particle_motion_data_render_vs.cso", "Data/shader/particle_motiom_blur_gs.cso", "Data/shader/particle_motion_blur_ps.cso", inputElementDesc, ARRAYSIZE(inputElementDesc));
#endif
	Load();
}
/**********************エディタ関数***************************/
void FireworksParticle::ImGuiUpdate()
{
#ifdef USE_IMGUI
	ImGui::Begin("fireworks particle");
	ImGui::Text("fireworks type");
	int i = 1;
	std::vector<std::string>fireworksName;
	if (ImGui::Button("New fireworks type"))
	{
		mFireworkDatas.emplace_back();
		memset(&mFireworkDatas.back(), 0, sizeof(FireworksData));
	}
	int deleteFireworksNumber = -1, deleteEmitorNumber = -1, startDeleteNumber = -1;
	ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(450, 300), ImGuiWindowFlags_NoTitleBar);

	for (auto& fireworks : mFireworkDatas)
	{
		fireworksName.push_back("fireworks" + std::to_string(i));
		i++;
		if (ImGui::CollapsingHeader(fireworksName.back().c_str()))
		{
			ImGui::InputFloat((fireworksName.back() + " max life").c_str(), &fireworks.maxLife, 1);
			ImGui::InputFloat((fireworksName.back() + " max count").c_str(), &fireworks.maxCount, 1);
			ImGui::InputFloat((fireworksName.back() + " scale").c_str(), &fireworks.scale, 0.1f);
			ImGui::InputFloat((fireworksName.back() + " speed").c_str(), &fireworks.speed, 1);
			ImGui::InputFloat((fireworksName.back() + " parsent").c_str(), &fireworks.parsent, 0.1f);
			ImGui::InputFloat((fireworksName.back() + " end timer").c_str(), &fireworks.endTimer, 0.1f);
			ImGui::InputFloat((fireworksName.back() + " gravity").c_str(), &fireworks.gravity, 0.1f);
			float* color[] = { &fireworks.color.x,&fireworks.color.y ,&fireworks.color.z };
			ImGui::ColorEdit3((fireworksName.back() + " in color").c_str(), *color);
			ImGui::SliderFloat((fireworksName.back() + " in color w").c_str(), &fireworks.color.w, 0, 1);
			float* endColor[] = { &fireworks.endColor.x,&fireworks.endColor.y ,&fireworks.endColor.z };
			ImGui::ColorEdit3((fireworksName.back() + " out color").c_str(), *endColor);
			ImGui::SliderFloat((fireworksName.back() + " out color w").c_str(), &fireworks.endColor.w, 0, 1);
			if (ImGui::Button((fireworksName.back() + "delete").c_str()))
			{
				deleteFireworksNumber = i - 2;
				for (auto& emitor : mEmitorData)
				{
					if (deleteFireworksNumber <= emitor.type)
					{
						emitor.type--;
					}
				}
			}
		}
	}
	ImGui::EndChild();

	/**********************emitorデータ**********************/
	ImGui::Text("emitor");

	ImGui::Checkbox("emitor parameter edit", &mParameterEditFlag);
	/****************************************************************/
	 //   最初に一回だけのemitorのデータ
	/****************************************************************/
	if (ImGui::CollapsingHeader("start emitor data"))
	{
		for (int i = 0; i < 5; i++)
		{
			std::string name = "rank:" + std::to_string(i + 1);
			ImGui::RadioButton(name.c_str(), &mDefRanking, i);
		}
		ImGui::InputFloat("start time line", &mStartMaxTime);
		ImGui::InputInt("one rank emitor count", &mOneRankEmitorCount, 1);
		int size = 5 * mOneRankEmitorCount + mOneRankEmitorCount / 2;
		if (size > static_cast<int>(mStartEmitorData.size()))
		{
			int s = size - mStartEmitorData.size();
			for (int i = 0; i < s; i++)
			{
				mStartEmitorData.emplace_back();
			}
		}
		else if (size < static_cast<int>(mStartEmitorData.size()))
		{
			int s = mStartEmitorData.size() - size;
			for (int i = 0; i < s; i++)
			{
				mStartEmitorData.erase(mStartEmitorData.begin() + size);
			}

		}

		ImGui::BeginChild(ImGui::GetID((void*)1), ImVec2(450, 300), ImGuiWindowFlags_NoTitleBar);
		i = 1;
		for (auto& emitor : mStartEmitorData)
		{
			std::string emitorName = "start emitor" + std::to_string(i);
			i++;
			ImGui::SliderFloat((emitorName + "time line").c_str(), &emitor.emitorStartTime, 0, mStartMaxTime);

			if (mParameterEditFlag)
			{
				if (ImGui::CollapsingHeader(emitorName.c_str()))
				{
					float* position[3] = { &emitor.position.x,&emitor.position.y,&emitor.position.z };
					ImGui::DragFloat3((emitorName + " position").c_str(), *position, 1);
					float* velocity[3] = { &emitor.velocity.x,&emitor.velocity.y,&emitor.velocity.z };
					ImGui::SliderFloat3((emitorName + " velocity").c_str(), *velocity, -1, 1);
					ImGui::InputFloat((emitorName + " max life").c_str(), &emitor.maxLife, 1);
					ImGui::InputFloat((emitorName + " speed").c_str(), &emitor.speed, 1);
					ImGui::Combo((emitorName + "fireworks").c_str(), &emitor.type, vector_getter, static_cast<void*>(&fireworksName), fireworksName.size());
					if (ImGui::Button((emitorName + "delete").c_str()))
					{
						startDeleteNumber = i - 2;
					}

				}
			}
			ImGui::Separator();
		}
		ImGui::EndChild();
	}
	/****************************************************************/
	//   ループするemitorのデータ
	/****************************************************************/

	if (ImGui::CollapsingHeader("loop emitor data"))
	{
		ImGui::InputFloat("max time line", &mMaxEmitorTime);
		if (ImGui::Button("New Emitor"))
		{
			mEmitorData.emplace_back();
		}

		ImGui::BeginChild(ImGui::GetID((void*)2), ImVec2(450, 300), ImGuiWindowFlags_NoTitleBar);
		i = 1;
		for (auto& emitor : mEmitorData)
		{
			std::string emitorName = "emitor" + std::to_string(i);
			i++;
			ImGui::SliderFloat((emitorName + "time line").c_str(), &emitor.emitorStartTime, 0, mMaxEmitorTime);
			//パラメーターフラグがtrueの時
			if (mParameterEditFlag)
			{
				if (ImGui::CollapsingHeader(emitorName.c_str()))
				{
					float* position[3] = { &emitor.position.x,&emitor.position.y,&emitor.position.z };
					ImGui::DragFloat3((emitorName + " position").c_str(), *position, 1);
					float* velocity[3] = { &emitor.velocity.x,&emitor.velocity.y,&emitor.velocity.z };
					ImGui::SliderFloat3((emitorName + " velocity").c_str(), *velocity, -1, 1);
					ImGui::InputFloat((emitorName + " max life").c_str(), &emitor.maxLife, 1);
					ImGui::InputFloat((emitorName + " speed").c_str(), &emitor.speed, 1);
					ImGui::Combo((emitorName + "fireworks").c_str(), &emitor.type, vector_getter, static_cast<void*>(&fireworksName), fireworksName.size());
					if (ImGui::Button((emitorName + "delete").c_str()))
					{
						deleteEmitorNumber = i - 2;
					}

				}
			}
			ImGui::Separator();
		}
		ImGui::EndChild();

	}
	if (deleteFireworksNumber != -1)mFireworkDatas.erase(mFireworkDatas.begin() + deleteFireworksNumber);

	if (deleteEmitorNumber != -1)mEmitorData.erase(mEmitorData.begin() + deleteEmitorNumber);

	if (startDeleteNumber != -1)mStartEmitorData.erase(mStartEmitorData.begin() + startDeleteNumber);

	if (ImGui::Button("save"))
	{
		Save();
	}
	ImGui::RadioButton("start rank fireworks",&mDefStartState,0);
	ImGui::RadioButton("start loop fireworks", &mDefStartState, 1);
	std::string name = mCreateFlag ? "end" : "start";
	if (ImGui::Button(name.c_str()))
	{
		if (mCreateFlag)
		{
			ClearEmitor();
		}
		else
		{
			CreateEmitor(mDefRanking);
			mState = mDefStartState;
			if (mState==1)
			{
				mEmitors.clear();
				mCreateCount.clear();
				mEmitors.resize(mEmitorData.size());
				mCreateCount.resize(mEmitorData.size());
				memset(&mCreateCount[0], 0, sizeof(int)* mCreateCount.size());
			}
		}
	}
	ImGui::End();
#endif
}
/************************更新********************************/
void FireworksParticle::Update(float elapsdTime, ID3D11DeviceContext* context)
{
	context->CSSetUnorderedAccessViews(0, 1, mParticleUAV.GetAddressOf(), nullptr);
	if (mCreateFlag)
	{
		context->CSSetConstantBuffers(0, 1, mCbCreateBuffer.GetAddressOf());
		CbCreate cbCreate, cbCreate2;
		memset(&cbCreate, 0, sizeof(CbCreate));
		memset(&cbCreate2, 0, sizeof(CbCreate));
		/*****************エミッタの更新*********************/
		int fireworksCount = 0;
		int emitorCount = 0;

		mEmitorTimer += elapsdTime;

		switch (mState)
		{
		case 0:
			StartFireworksEmitorUpdate(elapsdTime, &cbCreate2, &cbCreate, emitorCount, fireworksCount);
			break;
		case 1:
			LoopFireworksEmitorUpdate(elapsdTime, &cbCreate2, &cbCreate, emitorCount, fireworksCount);
			break;
		}
		/*********************花火のパーティクル生成*****************************/
		if (fireworksCount > 0)
		{
			context->CSSetShader(mCSCreate1Shader.Get(), nullptr, 0);
			int count = 0;
			for (auto& create : cbCreate.createData)
			{
				count += static_cast<int>(create.firework.maxCount);
			}
			if (mIndex + count >= mMaxParticle)
			{
				mIndex = 0;
			}
			cbCreate.startIndex = mIndex;
			context->UpdateSubresource(mCbCreateBuffer.Get(), 0, 0, &cbCreate, 0, 0);
			context->Dispatch(count, 1, 1);
			mIndex += count;
		}

		/*********************打ち上げ中のパーティクル生成*****************************/
		if (emitorCount > 0)
		{
			context->CSSetShader(mCSCreate2Shader.Get(), nullptr, 0);
			int count = 0;
			for (int i = 0; i < emitorCount; i++)
			{
				auto& data = cbCreate2.createData[i];
				float length = 0;
				DirectX::XMStoreFloat(&length, DirectX::XMVector3Length(DirectX::XMLoadFloat3(&data.velocity)));
				data.firework.maxCount = length / 0.05f;
				count += static_cast<int>(data.firework.maxCount);
			}
			if (mIndex + count >= mMaxParticle)
			{
				mIndex = 0;
			}
			cbCreate2.startIndex = mIndex;

			context->UpdateSubresource(mCbCreateBuffer.Get(), 0, 0, &cbCreate2, 0, 0);

			context->Dispatch(count, 1, 1);

			mIndex += count;
		}
	}
	/*********************パーティクルの更新*****************************/
	context->CSSetConstantBuffers(1, 1, mCbUpdateBuffer.GetAddressOf());
	context->CSSetUnorderedAccessViews(2, 1, mRenderUAV.GetAddressOf(), nullptr);

	CbUpdate cbUpdate;
	cbUpdate.elapsdime = elapsdTime;
	context->UpdateSubresource(mCbUpdateBuffer.Get(), 0, 0, &cbUpdate, 0, 0);
	context->CSSetShader(mCSShader.Get(), nullptr, 0);

	context->Dispatch(mMaxParticle / 100, 1, 1);

	ID3D11UnorderedAccessView* uav = nullptr;
	context->CSSetUnorderedAccessViews(2, 1, &uav, nullptr);
	context->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);

	context->CSSetShader(nullptr, nullptr, 0);
	ID3D11Buffer* buffer[] = { nullptr,nullptr };
	context->CSSetConstantBuffers(0, 2, buffer);

}
/*******************************描画*********************************/
void FireworksParticle::Render(ID3D11DeviceContext* context)
{
	mShader->Activate(context);

	u_int stride = sizeof(RenderParticle);
	u_int offset = 0;

	context->IASetVertexBuffers(0, 1, mRenderBuffer.GetAddressOf(), &stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	context->Draw(mMaxParticle, 0);
	mShader->Deactivate(context);

}
void FireworksParticle::Render(ID3D11DeviceContext* context, DrowShader* shader)
{
	shader->Activate(context);

	u_int stride = sizeof(RenderParticle);
	u_int offset = 0;

	context->IASetVertexBuffers(0, 1, mRenderBuffer.GetAddressOf(), &stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	context->Draw(mMaxParticle, 0);
	shader->Deactivate(context);

}
/*****************************ファイルロード***************************/
void FireworksParticle::Load()
{
	FILE* fp;
	if (fopen_s(&fp, "Data/file/fireworks_particle.bin", "rb") == 0)
	{
		int fireworksSize = 0, emitorSize = 0, startEditorSize = 0;
		fread(&fireworksSize, sizeof(int), 1, fp);
		fread(&emitorSize, sizeof(int), 1, fp);
		fread(&startEditorSize, sizeof(int), 1, fp);
		fread(&mMaxEmitorTime, sizeof(float), 1, fp);
		fread(&mStartMaxTime, sizeof(float), 1, fp);
		fread(&mOneRankEmitorCount, sizeof(int), 1, fp);
		if (fireworksSize != 0)
		{
			mFireworkDatas.resize(fireworksSize);
			fread(&mFireworkDatas[0], sizeof(FireworksData), fireworksSize, fp);
		}
		if (emitorSize != 0)
		{
			mEmitorData.resize(emitorSize);
			fread(&mEmitorData[0], sizeof(EmitorData), emitorSize, fp);
		}
		if (startEditorSize != 0)
		{
			mStartEmitorData.resize(startEditorSize);
			fread(&mStartEmitorData[0], sizeof(EmitorData), startEditorSize, fp);
		}

		fclose(fp);
	}
}
/************************ファイルセーブ*******************************/
void FireworksParticle::Save()
{
	FILE* fp;
	fopen_s(&fp, "Data/file/fireworks_particle.bin", "wb");
	int fireworksSize = mFireworkDatas.size(), emitorSize = mEmitorData.size(), startEditorSize = mStartEmitorData.size();
	fwrite(&fireworksSize, sizeof(int), 1, fp);
	fwrite(&emitorSize, sizeof(int), 1, fp);
	fwrite(&startEditorSize, sizeof(int), 1, fp);
	fwrite(&mMaxEmitorTime, sizeof(float), 1, fp);
	fwrite(&mStartMaxTime, sizeof(float), 1, fp);
	fwrite(&mOneRankEmitorCount, sizeof(int), 1, fp);
	if (fireworksSize != 0)
	{
		fwrite(&mFireworkDatas[0], sizeof(FireworksData), fireworksSize, fp);
	}
	if (emitorSize != 0)
	{
		fwrite(&mEmitorData[0], sizeof(EmitorData), emitorSize, fp);
	}
	if (startEditorSize != 0)
	{
		fwrite(&mStartEmitorData[0], sizeof(EmitorData), startEditorSize, fp);
	}
	fclose(fp);

}

void FireworksParticle::StartFireworksEmitorUpdate(float elapsdTime, CbCreate* smokeConstant, CbCreate* fireworksConstant, int& emitorCount, int& fireworksCount)
{
	for (int i = 0; i < static_cast<int>(mStartCreateNumberList.size()); i++)
	{
		auto& emitorData = mStartEmitorData[mStartCreateNumberList[i]];
		auto& emitor = mEmitors[i];

		if (emitorData.emitorStartTime > mEmitorTimer)continue;
		if (emitor.type == -1)
		{
			if (mCreateCount[i] > 0)continue;

			emitor.position = emitorData.position;
			emitor.velocity = emitorData.velocity * emitorData.speed;
			emitor.life = emitorData.maxLife;
			emitor.type = emitorData.type;
			mCreateCount[i]++;
		}
		emitor.position += emitor.velocity * elapsdTime;
		emitor.life -= elapsdTime;
		if (emitor.life <= 0)
		{
			fireworksConstant->createData[fireworksCount].position = emitor.position;
			fireworksConstant->createData[fireworksCount].velocity = emitor.velocity;
			fireworksConstant->createData[fireworksCount].firework = mFireworkDatas[emitor.type];
			emitor.type = -1;
			fireworksCount++;
		}
		else
		{
			smokeConstant->createData[emitorCount].position = emitor.position;
			smokeConstant->createData[emitorCount].velocity = emitor.velocity * elapsdTime;
			smokeConstant->createData[emitorCount].firework.color = VECTOR4F(1, 1, 1, 1);
			smokeConstant->createData[emitorCount].firework.endTimer = 0.25f;
			smokeConstant->createData[emitorCount].firework.scale = 0.5f;
			emitorCount++;
		}


	}
	if (mEmitorTimer >= mStartMaxTime)
	{
		mState++;
		mEmitorTimer = 0;
		mEmitors.clear();
		mCreateCount.clear();
		mEmitors.resize(mEmitorData.size());
		mCreateCount.resize(mEmitorData.size());
		memset(&mCreateCount[0], 0, sizeof(int) * mCreateCount.size());

	}
}

void FireworksParticle::LoopFireworksEmitorUpdate(float elapsdTime, CbCreate* smokeConstant, CbCreate* fireworksConstant, int& emitorCount, int& fireworksCount)
{
	for (int i = 0; i < static_cast<int>(mEmitors.size()); i++)
	{
		auto& emitor = mEmitors[i];
		auto& emitorData = mEmitorData[i];
		if (emitor.type == -1)
		{
			if (mCreateCount[i] > 0)continue;
			if (emitorData.emitorStartTime > mEmitorTimer)continue;

			emitor.position = emitorData.position;
			emitor.velocity = emitorData.velocity * emitorData.speed;
			emitor.life = emitorData.maxLife;
			emitor.type = emitorData.type;
			mCreateCount[i]++;
		}
		emitor.position += emitor.velocity * elapsdTime;
		emitor.life -= elapsdTime;
		if (emitor.life <= 0)
		{
			fireworksConstant->createData[fireworksCount].position = emitor.position;
			fireworksConstant->createData[fireworksCount].velocity = emitor.velocity;
			fireworksConstant->createData[fireworksCount].firework = mFireworkDatas[emitor.type];
			emitor.type = -1;
			fireworksCount++;
		}
		else
		{
			smokeConstant->createData[emitorCount].position = emitor.position;
			smokeConstant->createData[emitorCount].velocity = emitor.velocity * elapsdTime;
			smokeConstant->createData[emitorCount].firework.color = VECTOR4F(1, 1, 1, 1);
			smokeConstant->createData[emitorCount].firework.endTimer = 0.25f;
			smokeConstant->createData[emitorCount].firework.scale = 0.5f;
			
			emitorCount++;
		}
	}

	if (mEmitorTimer >= mMaxEmitorTime)
	{
		//mEmitors.clear();
		//mCreateFlag = false;
		memset(&mCreateCount[0], 0, sizeof(int) * mCreateCount.size());

		mEmitorTimer = 0;
		mFireworksCount = 0;
	}
	//else if (mFireworksCount >= mCreateCount.size())
	//{
	//	memset(&mCreateCount[0], 0, sizeof(int) * mCreateCount.size());

	//	mFireworksCount = 0;
	//	mEmitorTimer = 0;

	//}

}

void FireworksParticle::SetStartList(const int size)
{
	mStartCreateNumberList.clear();
	for (int i = 0; i < static_cast<int>(mStartEmitorData.size()); i++)
	{
		mStartCreateNumberList.push_back(i);
	}
	static int randNo = 0;
	while (1)
	{
		if (mStartCreateNumberList.size() == size)break;

		randNo = rand() % mStartEmitorData.size();

		auto randomCount = [](int x) {return x == randNo; };

		auto randomDelete = std::remove_if(mStartCreateNumberList.begin(), mStartCreateNumberList.end(), randomCount);

		mStartCreateNumberList.erase(randomDelete, mStartCreateNumberList.end());

	}
}
