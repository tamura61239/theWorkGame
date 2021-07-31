#include "fireworks_particle.h"
#include<string>
#include"misc.h"
#include"shader.h"
#include"vector_combo.h"
#include"texture.h"

#ifdef USE_IMGUI
#include<imgui.h>
#endif
/*****************************************************/
//　　　　　　　　　　初期化関数(コンストラクタ)
/*****************************************************/

FireworksParticle::FireworksParticle(ID3D11Device* device) :mCreateFlag(false), mMaxParticle(350000)
, mEmitorTimer(0), mParameterEditFlag(false), mIndex(0), mDefRanking(0), mNowPlayRanking(0), 
mState(0), mDefStartState(0), mFireworksCount(0)
{
	HRESULT hr;

	{
		//パーティクルのバッファの生成
		std::vector<Particle>particles;
		particles.resize(mMaxParticle);
		mParticle = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, particles, true);
		//描画用バッファの生成
		std::vector<RenderParticle>renderParticles;
		renderParticles.resize(mMaxParticle);
		mParticleRender = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS, renderParticles, true);
		//indexバッファ
		std::vector<UINT>indices;
		indices.resize(mMaxParticle);
		for (int i = 0; i < 2; i++)mParticleIndexs[i] = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS, indices, true);
		//deleteindexバッファ
		for (int i = 0; i < mMaxParticle; i++)
		{
			indices[i] = i;
		}
		mParticleDeleteIndex = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, indices, true);
		//カウントバッファ
		ParticleCount particleCount;
		memset(&particleCount, 0, sizeof(particleCount));
		particleCount.deActiveParticleCount = mMaxParticle;
		mParticleCount = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS, particleCount, true, false, D3D11_CPU_ACCESS_READ);
	}
	//定数バッファの生成
	mCbCreate = std::make_unique<ConstantBuffer<CbCreate>>(device);
	mCbUpdate = std::make_unique<ConstantBuffer<CbUpdate>>(device);
	//コンピュートシェーダーの生成
	CreateCSFromCso(device, "Data/shader/fireworks_particle_create_cs.cso", mCSCreate1Shader.GetAddressOf());
	CreateCSFromCso(device, "Data/shader/fireworks_smoke_particle_create.cso", mCSCreate2Shader.GetAddressOf());
	CreateCSFromCso(device, "Data/shader/fireworks_particle_cs.cso", mCSShader.GetAddressOf());
	CreateCSFromCso(device, "Data/shader/particle_count_clear_cs.cso", mCSClearShader.GetAddressOf());
	CreateCSFromCso(device, "Data/shader/particle_count_cs.cso", mCSCountShader.GetAddressOf());
	//描画用シェーダーの生成
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"ANGLE",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"VELOCITY",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"SCALE",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
	};
	mShader = std::make_unique<DrowShader>(device, "Data/shader/particle_motion_data_render_vs.cso", "Data/shader/particle_motiom_blur_gs.cso", "Data/shader/particle_motion_blur_text_ps.cso", inputElementDesc, ARRAYSIZE(inputElementDesc));
	//描画時に使うテクスチャの生成
	wchar_t* names[] =
	{
		L"Data/image/○.png",
		L"",
		L"Data/image/無題1.png",
		L"Data/image/無題2.png",
		L"Data/image/無題3.png",
		L"Data/image/無題4.png",
		L"Data/image/無題5.png",
		L"Data/image/無題6.png",
		L"Data/image/無題7.png",
		L"Data/image/無題8.png",
	};
	for (auto& name : names)
	{
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>srv;
		if (wcscmp(name, L"") == 0)
		{
			hr = MakeDummyTexture(device, srv.GetAddressOf());
		}
		else
		{
			hr = LoadTextureFromFile(device, name, srv.GetAddressOf());
		}
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		mParticleSRV.push_back(srv);
	}

	Load();
}
/*****************************************************/
//　　　　　　　　　　エディタ関数
/*****************************************************/

void FireworksParticle::Editor()
{
#ifdef USE_IMGUI
	ImGui::Begin("fireworks particle");
	//花火の種類
	ImGui::Text("fireworks type");
	int i = 1;
	std::vector<std::string>fireworksName;
	//新しい種類の花火を生成
	if (ImGui::Button("New fireworks type"))
	{
		mFireworkDatas.emplace_back();
		memset(&mFireworkDatas.back(), 0, sizeof(FireworksData));
	}
	int deleteFireworksNumber = -1, deleteEmitorNumber = -1, startDeleteNumber = -1;
	ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(0, 300), ImGuiWindowFlags_NoTitleBar);

	for (auto& fireworks : mFireworkDatas)
	{
		fireworksName.push_back("fireworks" + std::to_string(i));
		i++;
		//種類ごとにパラメーターを調整する
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
			//消去
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

	//    emitorデータ
	ImGui::Text("emitor");

	ImGui::Checkbox("emitor parameter edit", &mParameterEditFlag);
	 //   最初に一回だけのemitorのデータ
	if (ImGui::CollapsingHeader("start emitor data"))
	{
		for (int i = 0; i < 5; i++)
		{
			std::string name = "rank:" + std::to_string(i + 1);
			ImGui::RadioButton(name.c_str(), &mDefRanking, i);
		}
		//開始時間の設定
		ImGui::InputFloat("start time line", &mEditorData.mStartMaxTime);
		//ランキングが一つ上がるごとに増やすエミッターの数
		ImGui::InputInt("one rank emitor count", &mEditorData.mOneRankEmitorCount, 1);
		int size = 5 * mEditorData.mOneRankEmitorCount + mEditorData.mOneRankEmitorCount / 2;
		//最大生成数がエミッターの数を超えたら
		if (size > static_cast<int>(mStartEmitorData.size()))
		{
			int s = size - mStartEmitorData.size();
			for (int i = 0; i < s; i++)
			{
				mStartEmitorData.emplace_back();
			}
		}
		//最大生成数がエミッター数より少なかったら
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
			//花火の打ち上げ開始時間
			ImGui::SliderFloat((emitorName + "time line").c_str(), &emitor.emitorStartTime, 0, mEditorData.mStartMaxTime);

			if (mParameterEditFlag)
			{
				if (ImGui::CollapsingHeader(emitorName.c_str()))
				{
					//パラメーターを調整する
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
	//   ループするemitorのデータ

	if (ImGui::CollapsingHeader("loop emitor data"))
	{
		//最大時間
		ImGui::InputFloat("max time line", &mEditorData.mMaxEmitorTime);
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
			//開始時間
			ImGui::SliderFloat((emitorName + "time line").c_str(), &emitor.emitorStartTime, 0, mEditorData.mMaxEmitorTime);
			//パラメーターフラグがtrueの時
			if (mParameterEditFlag)
			{
				if (ImGui::CollapsingHeader(emitorName.c_str()))
				{
					//パラメーターを調整する
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
	//消す
	if (deleteFireworksNumber != -1)mFireworkDatas.erase(mFireworkDatas.begin() + deleteFireworksNumber);

	if (deleteEmitorNumber != -1)mEmitorData.erase(mEmitorData.begin() + deleteEmitorNumber);

	if (startDeleteNumber != -1)mStartEmitorData.erase(mStartEmitorData.begin() + startDeleteNumber);
	//セーブ
	if (ImGui::Button("save"))
	{
		Save();
	}
	ImGui::RadioButton("start rank fireworks",&mDefStartState,0);
	ImGui::RadioButton("start loop fireworks", &mDefStartState, 1);
	//開始と終了
	std::string name = mCreateFlag ? "end" : "start";
	if (ImGui::Button(name.c_str()))
	{
		if (mCreateFlag)
		{//終了
			ClearEmitor();
		}
		else
		{//開始
			CreateEmitor(mDefRanking);
			mState = -1;
			if (mDefStartState ==1)
			{//初期化
				mEmitors.clear();
				mCreateCount.clear();
				mEmitors.resize(mEmitorData.size());
				mCreateCount.resize(mEmitorData.size());
				memset(&mCreateCount[0], 0, sizeof(int)* mCreateCount.size());
			}
		}
	}
	ImVec2 size = ImVec2(75, 75);
	for (UINT i = 0; i < static_cast<UINT>(mParticleSRV.size()); i++)
	{
		//描画時に使うテクスチャを指定
		if (ImGui::ImageButton(mParticleSRV[i].Get(), size))
		{
			mEditorData.textureType = i;
		}
		if (i % 4 < 3 && i < static_cast<UINT>(mParticleSRV.size() - 1))ImGui::SameLine();
	}
	ImGui::Text(u8"今のテクスチャ");
	size = ImVec2(150, 150);
	ImGui::Image(mParticleSRV[mEditorData.textureType].Get(), size);

	ImGui::End();
#endif
}
/*****************************************************/
//　　　　　　　　　　更新関数
/*****************************************************/

/****************************パーティクルの更新******************************/
void FireworksParticle::Update(float elapsdTime, ID3D11DeviceContext* context)
{
	//UAVをGPUに渡す
	mParticle->Activate(context, 0, true);
	mParticleCount->Activate(context, 1, true);
	mParticleRender->Activate(context, 2, true);
	mParticleIndexs[mIndexNum]->Activate(context, 3, true);
	mParticleIndexs[1 - mIndexNum]->Activate(context, 4, true);
	mParticleDeleteIndex->Activate(context, 5, true);
	mIndexNum++;
	if (mIndexNum >= 2)mIndexNum = 0;
	if (mCreateFlag)
	{
		CbCreate fireworksConstant, smokeConstant;
		memset(&fireworksConstant, 0, sizeof(CbCreate));
		memset(&smokeConstant, 0, sizeof(CbCreate));
		//エミッタの更新
		int fireworksCount = 0;
		int emitorCount = 0;

		//エミッターの更新
		mEmitorTimer += elapsdTime;
		switch (mState)
		{
		case -1:
			Clearount(context);
		case 0://最初だけ
			StartFireworksEmitorUpdate(elapsdTime, &smokeConstant, &fireworksConstant, emitorCount, fireworksCount);
			break;
		case 1://ループ
			LoopFireworksEmitorUpdate(elapsdTime, &smokeConstant, &fireworksConstant, emitorCount, fireworksCount);
			break;
		}
		//花火のパーティクル生成
		if (fireworksCount > 0)
		{
			context->CSSetShader(mCSCreate1Shader.Get(), nullptr, 0);
			int count = 0;
			//生成するパーティクルの数を計算している
			for (auto& create : fireworksConstant.createData)
			{
				count += static_cast<int>(create.firework.maxCount);
			}
			//上限に達したらindexを0に戻す
			if (mIndex + count >= mMaxParticle)
			{
				mIndex = 0;
			}
			//定数バッファをGPUに設定
			mCbCreate->data = fireworksConstant;
			mCbCreate->data.startIndex = mIndex;
			mCbCreate->Activate(context, 0,false, false, false, true);
			context->Dispatch(count, 1, 1);
			mIndex += count;
			//GPUから解放
			mCbCreate->DeActivate(context);
		}

		//打ち上げ中(煙)のパーティクル生成
		if (emitorCount > 0)
		{
			context->CSSetShader(mCSCreate2Shader.Get(), nullptr, 0);
			int count = 0;
			//生成するパーティクルの数を計算している
			for (int i = 0; i < emitorCount; i++)
			{
				auto& data = smokeConstant.createData[i];
				float length = 0;
				DirectX::XMStoreFloat(&length, DirectX::XMVector3Length(DirectX::XMLoadFloat3(&data.velocity)));
				data.firework.maxCount = length / 0.05f;
				count += static_cast<int>(data.firework.maxCount);
			}
			//上限に達したらindexを0に戻す
			if (mIndex + count >= mMaxParticle)
			{
				mIndex = 0;
			}
			//定数バッファをGPUに設定
			mCbCreate->data = smokeConstant;
			mCbCreate->data.startIndex = mIndex;
			mCbCreate->Activate(context,0, false, false, false, true);
			context->Dispatch(count, 1, 1);
			mIndex += count;
			//GPUから解放
			mCbCreate->DeActivate(context);

		}
	}
	//   パーティクルの更新
	//定数バッファをGPUに設定
	mCbUpdate->data.elapsdime = elapsdTime;
	mCbUpdate->Activate(context, 1, false, false, false, true);
	context->CSSetShader(mCSShader.Get(), nullptr, 0);

	context->Dispatch(mMaxParticle / 100, 1, 1);
	//GPUから解放
	mCbUpdate->DeActivate(context);
	//パーティクル数の更新
	context->CSSetShader(mCSCountShader.Get(), nullptr, 0);
	context->Dispatch(1, 1, 1);

	//カウントを取得
	D3D11_MAPPED_SUBRESOURCE ms;
	context->Map(mParticleCount->GetBuffer(), NULL, D3D11_MAP_READ, NULL, &ms);
	ParticleCount* particleCount = (ParticleCount*)ms.pData;
	mRenderCount = particleCount->aliveParticleCount;
	context->Unmap(mParticleCount->GetBuffer(), NULL);
	//GPUのデータを解放
	context->CSSetShader(nullptr, nullptr, 0);
	mParticle->DeActivate(context);
	mParticleCount->DeActivate(context);
	mParticleRender->DeActivate(context);
	mParticleIndexs[mIndexNum]->DeActivate(context);
	mParticleIndexs[1 - mIndexNum]->DeActivate(context);
	mParticleDeleteIndex->DeActivate(context);

}
/****************************開始時に一回だけ出るパーティクルのエミッター******************************/

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
			//エミッター生成
			emitor.position = emitorData.position;
			emitor.velocity = emitorData.velocity * emitorData.speed;
			emitor.life = emitorData.maxLife;
			emitor.type = emitorData.type;
			mCreateCount[i]++;
		}
		emitor.position += emitor.velocity * elapsdTime;
		emitor.life -= elapsdTime;
		if (emitor.life <= 0)
		{//花火のパーティクルのエミッター
			fireworksConstant->createData[fireworksCount].position = emitor.position;
			fireworksConstant->createData[fireworksCount].velocity = emitor.velocity;
			fireworksConstant->createData[fireworksCount].firework = mFireworkDatas[emitor.type];
			emitor.type = -1;
			fireworksCount++;
		}
		else
		{//打ち上げ中の煙のパーティクル
			smokeConstant->createData[emitorCount].position = emitor.position;
			smokeConstant->createData[emitorCount].velocity = emitor.velocity * elapsdTime;
			smokeConstant->createData[emitorCount].firework.color = VECTOR4F(1, 1, 1, 1);
			smokeConstant->createData[emitorCount].firework.endTimer = 0.25f;
			smokeConstant->createData[emitorCount].firework.scale = 0.5f;
			emitorCount++;
		}


	}
	if (mEmitorTimer >= mEditorData.mStartMaxTime)
	{//スタート時のエミッターの生成の最大時間を過ぎたら
		mState++;
		mEmitorTimer = 0;
		mEmitors.clear();
		mCreateCount.clear();
		mEmitors.resize(mEmitorData.size());
		mCreateCount.resize(mEmitorData.size());
		memset(&mCreateCount[0], 0, sizeof(int) * mCreateCount.size());

	}
}
/****************************花火のエミッターのループ******************************/

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
			//エミッター生成
			emitor.position = emitorData.position;
			emitor.velocity = emitorData.velocity * emitorData.speed;
			emitor.life = emitorData.maxLife;
			emitor.type = emitorData.type;
			mCreateCount[i]++;
		}
		emitor.position += emitor.velocity * elapsdTime;
		emitor.life -= elapsdTime;
		if (emitor.life <= 0)
		{//花火のパーティクルのエミッター
			fireworksConstant->createData[fireworksCount].position = emitor.position;
			fireworksConstant->createData[fireworksCount].velocity = emitor.velocity;
			fireworksConstant->createData[fireworksCount].firework = mFireworkDatas[emitor.type];
			emitor.type = -1;
			fireworksCount++;
		}
		else
		{//打ち上げ中の煙のパーティクルのエミッター
			smokeConstant->createData[emitorCount].position = emitor.position;
			smokeConstant->createData[emitorCount].velocity = emitor.velocity * elapsdTime;
			smokeConstant->createData[emitorCount].firework.color = VECTOR4F(1, 1, 1, 1);
			smokeConstant->createData[emitorCount].firework.endTimer = 0.25f;
			smokeConstant->createData[emitorCount].firework.scale = 0.5f;

			emitorCount++;
		}
	}

	if (mEmitorTimer >= mEditorData.mMaxEmitorTime)
	{//エミッターのループの最大時間を過ぎたら
		memset(&mCreateCount[0], 0, sizeof(int) * mCreateCount.size());

		mEmitorTimer = 0;
		mFireworksCount = 0;
	}

}
/****************************エミッターの数を指定された分まで減らす******************************/

void FireworksParticle::SetStartList(const int size)
{

	mStartCreateNumberList.clear();
	for (int i = 0; i < static_cast<int>(mStartEmitorData.size()); i++)
	{
		mStartCreateNumberList.push_back(i);
	}
	//エミッターの数を減らす
	static int randNo = 0;
	while (1)
	{
		//指定されたサイズまでなったら終了
		if (mStartCreateNumberList.size() == size)break;

		randNo = rand() % mStartEmitorData.size();

		auto randomCount = [](int x) {return x == randNo; };

		auto randomDelete = std::remove_if(mStartCreateNumberList.begin(), mStartCreateNumberList.end(), randomCount);

		mStartCreateNumberList.erase(randomDelete, mStartCreateNumberList.end());

	}
}
/***************************************パーティクルカウントを初期化する*****************************************/
void FireworksParticle::Clearount(ID3D11DeviceContext* context)
{
	context->CSSetShader(mCSClearShader.Get(), nullptr, 0);

	context->Dispatch(1, 1, 1);

	mState = mDefStartState;
}

/*****************************************************/
//　　　　　　　　　　描画関数
/*****************************************************/

/****************************描画(シェーダーを取得してない)******************************/
void FireworksParticle::Render(ID3D11DeviceContext* context)
{
	//シェーダーの設定
	mShader->Activate(context);
	//描画時に使うテクスチャの設定
	context->PSSetShaderResources(0, 1, mParticleSRV[mEditorData.textureType].GetAddressOf());
	//バッファの設定
	u_int stride = sizeof(RenderParticle);
	u_int offset = 0;
	ID3D11Buffer* buffer = mParticleRender->GetBuffer();
	ID3D11Buffer* index = mParticleIndexs[mIndexNum]->GetBuffer();
	context->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
	context->IASetIndexBuffer(index, DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	context->DrawIndexed(mRenderCount, 0,0);
	//GPUからの解放
	mShader->Deactivate(context);
	ID3D11ShaderResourceView* srv = nullptr;
	context->PSSetShaderResources(0, 1, &srv);
	buffer = nullptr;
	index = nullptr;
	stride = 0;
	context->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
	context->IASetIndexBuffer(index, DXGI_FORMAT_R32_UINT, 0);

}
/****************************描画(シェーダーを取得する)******************************/

void FireworksParticle::Render(ID3D11DeviceContext* context, DrowShader* shader)
{
	//シェーダーの設定
	shader->Activate(context);
	//描画時に使うテクスチャの設定
	context->PSSetShaderResources(0, 1, mParticleSRV[mEditorData.textureType].GetAddressOf());
	//バッファの設定
	u_int stride = sizeof(RenderParticle);
	u_int offset = 0;
	ID3D11Buffer* buffer = mParticleRender->GetBuffer();
	ID3D11Buffer* index = mParticleIndexs[mIndexNum]->GetBuffer();
	context->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
	context->IASetIndexBuffer(index, DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	context->DrawIndexed(mRenderCount, 0, 0);
	//GPUからの解放
	shader->Deactivate(context);
	ID3D11ShaderResourceView* srv = nullptr;
	context->PSSetShaderResources(0, 1, &srv);
	buffer = nullptr;
	index = nullptr;
	stride = 0;
	context->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
	context->IASetIndexBuffer(index, DXGI_FORMAT_R32_UINT, 0);

}
/*****************************************************/
//　　　　　　　　　　初期化関数
/*****************************************************/

/****************************ロード******************************/
void FireworksParticle::Load()
{
	FILE* fp;
	if (fopen_s(&fp, "Data/file/fireworks_particle.bin", "rb") == 0)
	{
		long size = 0;
		fseek(fp, 0, SEEK_END);
		size = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		int fireworksSize = 0, emitorSize = 0, startEditorSize = 0;
		fread(&fireworksSize, sizeof(int), 1, fp);
		size -= sizeof(int);
		fread(&emitorSize, sizeof(int), 1, fp);
		size -= sizeof(int);
		fread(&startEditorSize, sizeof(int), 1, fp);
		size -= sizeof(int);
		if (fireworksSize != 0)
		{
			mFireworkDatas.resize(fireworksSize);
			fread(&mFireworkDatas[0], sizeof(FireworksData), fireworksSize, fp);
			size -= sizeof(FireworksData)* fireworksSize;
		}
		if (emitorSize != 0)
		{
			mEmitorData.resize(emitorSize);
			fread(&mEmitorData[0], sizeof(EmitorData), emitorSize, fp);
			size -= sizeof(EmitorData) * emitorSize;
		}
		if (startEditorSize != 0)
		{
			mStartEmitorData.resize(startEditorSize);
			fread(&mStartEmitorData[0], sizeof(EmitorData), startEditorSize, fp);
			size -= sizeof(EmitorData) * startEditorSize;

		}
		fread(&mEditorData, size, 1, fp);

		fclose(fp);
	}
}
/************************セーブ*******************************/
void FireworksParticle::Save()
{
	FILE* fp;
	fopen_s(&fp, "Data/file/fireworks_particle.bin", "wb");
	int fireworksSize = mFireworkDatas.size(), emitorSize = mEmitorData.size(), startEditorSize = mStartEmitorData.size();
	fwrite(&fireworksSize, sizeof(int), 1, fp);
	fwrite(&emitorSize, sizeof(int), 1, fp);
	fwrite(&startEditorSize, sizeof(int), 1, fp);
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
	fwrite(&mEditorData, sizeof(EditorData), 1, fp);

	fclose(fp);

}

