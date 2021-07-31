#include "respond_particle.h"
#include"misc.h"
#include"shader.h"
#include"texture.h"
#include"file_function.h"
#include"camera_manager.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif
/*****************************************************/
//　　　　　　　　　　初期化関数(コンストラクタ)
/*****************************************************/
RespondParticle::RespondParticle(ID3D11Device* device, std::shared_ptr<PlayerAI> player) :mCreateFlag(false), mTimer(0)
{
	//定数バッファ
	mCbBoneBuffer = std::make_unique<ConstantBuffer<CbBone>>(device);
	mCbCreateBuffer = std::make_unique<ConstantBuffer<CbCreate>>(device);
	mCbUpdateBuffer = std::make_unique<ConstantBuffer<CbUpdate>>(device);
	mCbCreateBuffer->data.respondPosition = player->GetCharacter()->GetPosition();
	int totalIndex = 0;
	HRESULT hr;
	mPlayer = player;
	//メッシュのバッファとSRV
	for (auto& mesh : player->GetCharacter()->GetModel()->GetModelResource()->GetMeshDatas())
	{
		mMeshs.emplace_back();
		mMeshs.back().mIndex = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, mesh.indices, false, true);
		mMeshs.back().mVertex = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, mesh.vertices, false, true);
		mMeshs.back().mMeshSize = mesh.indices.size() / 3;
		totalIndex += mesh.indices.size();
		//memcpy(&mMeshs.back().inverse.boneTransForm[0], &mesh.inverseTransforms[0], sizeof(FLOAT4X4) * mesh.inverseTransforms.size());

	}
	//パーティクルの最大数を計算
	int count = totalIndex / 10000;
	mMaxParticle = 1000000 * (count + 1);
	{
		//パーティクルのバッファ
		std::vector<Particle>particles;
		particles.resize(mMaxParticle);
		mParticle = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, particles, true);
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
		//描画用バッファ
		std::vector<RenderParticle>renderParticles;
		renderParticles.resize(mMaxParticle);
		mParticleRender = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS, renderParticles, true);
		//カウントバッファ
		ParticleCount particleCount;
		memset(&particleCount, 0, sizeof(particleCount));
		particleCount.deActiveParticleCount = mMaxParticle;
		mParticleCount = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS, particleCount, true, false, D3D11_CPU_ACCESS_READ);
	}
	//コンピュートシェーダーの生成
	hr = CreateCSFromCso(device, "Data/shader/respond_particle_create_cs.cso", mCreateCSShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = CreateCSFromCso(device, "Data/shader/respond_particle_cs.cso", mCSShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = CreateCSFromCso(device, "Data/shader/particle_count_cs.cso", mStartCSShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//描画用データの生成
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"ANGLE",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"VELOCITY",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"SCALE",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
	};
	mShader = std::make_unique<DrowShader>(device, "Data/shader/particle_render_vs.cso", "Data/shader/particle_render_cube_mesh_gs.cso", "Data/shader/particle_render_ps.cso", inputElementDesc, ARRAYSIZE(inputElementDesc));
	//エディタデータの初期化
	mEditorData.expansionTime = 10.3f;
	mEditorData.respondTime = 0.7f;
	mEditorData.expansionSpeed = 1.0f;
	mEditorData.mColor[0] = 1;
	mEditorData.mColor[1] = 1;
	mEditorData.mColor[2] = 1;
	mEditorData.mColor[3] = 1;
	mEditorData.scale = 0.5f;
	mEditorData.centerY = 3.0f;
	//
	mPlayer = player;
	//ファイルからデータのロード
	FileFunction::Load(mEditorData, "Data/file/respond_particle.bin", "rb");

}
/*****************************************************/
//　　　　　　　　　　エディタ関数
/*****************************************************/

void RespondParticle::Editor()
{
#ifdef USE_IMGUI
	ImGui::Begin("respond particle");
	ImGui::ColorEdit4("color", mEditorData.mColor);
	ImGui::DragFloat(u8"リスポンの時間", &mEditorData.respondTime);
	ImGui::DragFloat(u8"死亡演出の時間", &mEditorData.expansionTime);
	ImGui::DragFloat(u8"死亡演出時の速度", &mEditorData.expansionSpeed);
	ImGui::DragFloat(u8"サイズ", &mEditorData.scale);
	ImGui::DragFloat(u8"Y", &mEditorData.centerY);
	ImGui::Text(u8"タイム%f", mTimer);
	ImGui::Text(u8"生存数%d", mRenderCount);
	if (ImGui::Button("save"))FileFunction::Save(mEditorData, "Data/file/respond_particle.bin", "wb");
	ImGui::End();
#endif
}
/*****************************************************/
//　　　　　　　　　　更新関数
/*****************************************************/

void RespondParticle::Update(ID3D11DeviceContext* context, float elapsd_time)
{

	auto& player = mPlayer.lock();
	//SRVとUAVをGPUに送る
	mParticle->Activate(context, 0, true);
	mParticleCount->Activate(context, 1, true);
	mParticleRender->Activate(context, 2, true);
	mParticleIndexs[mIndexNum]->Activate(context, 3, true);
	mParticleIndexs[1 - mIndexNum]->Activate(context, 4, true);
	mParticleDeleteIndex->Activate(context, 5, true);
	mIndexNum++;
	if (mIndexNum >= 2)mIndexNum = 0;
	//生成
	Create(context, player);
	//更新
	Move(context, player, elapsd_time);
	//GPUに送ったデータの解放
	mParticle->DeActivate(context);
	mParticleCount->DeActivate(context);
	mParticleRender->DeActivate(context);
	mParticleIndexs[mIndexNum]->DeActivate(context);
	mParticleIndexs[1 - mIndexNum]->DeActivate(context);
	mParticleDeleteIndex->DeActivate(context);
}
/************************パーティクルの生成関数*****************************/

void RespondParticle::Create(ID3D11DeviceContext* context, std::shared_ptr<PlayerAI>player)
{
	if (player->GetCharacter()->GetExist() || mCreateFlag)return;
	mCreateFlag = true;
	const ModelResource* resouce = player->GetCharacter()->GetModel()->GetModelResource();
	const std::vector<Model::Node>& nodes = player->GetCharacter()->GetModel()->GetNodes();
	//生成情報のセット
	mCbCreateBuffer->data.expansionspeed = mEditorData.expansionSpeed;
	mCbCreateBuffer->data.expansionTime = mEditorData.expansionTime;
	mCbCreateBuffer->data.respondTime = mEditorData.respondTime;
	mCbCreateBuffer->data.color = 0;
	mCbCreateBuffer->data.color |= (static_cast<UINT>(mEditorData.mColor[0] * 255) & 0x00FFFFFF) << 24;
	mCbCreateBuffer->data.color |= (static_cast<UINT>(mEditorData.mColor[1] * 255) & 0x00FFFFFF) << 16;
	mCbCreateBuffer->data.color |= (static_cast<UINT>(mEditorData.mColor[2] * 255) & 0x00FFFFFF) << 8;
	mCbCreateBuffer->data.color |= (static_cast<UINT>(mEditorData.mColor[3] * 255) & 0x00FFFFFF) << 0;
	mCbCreateBuffer->data.scale = mEditorData.scale;
	mCbCreateBuffer->data.centerY = mEditorData.centerY;
	context->CSSetShader(mCreateCSShader.Get(), nullptr, 0);
	for (int i = 0; i < static_cast<int>(mMeshs.size()); i++)
	{
		const auto& mesh = mMeshs[i];
		const auto& boneData = resouce->GetMeshes()[i];
		if (boneData.nodeIndices.size() > 0)
		{
			//ボーンのワールド行列の計算
			for (int j = 0; j < static_cast<int>(boneData.nodeIndices.size()); j++)
			{
				DirectX::XMMATRIX inverseTransform = DirectX::XMLoadFloat4x4(boneData.inverseTransforms[j]);
				DirectX::XMMATRIX worldTransform = DirectX::XMLoadFloat4x4(&nodes[boneData.nodeIndices[j]].worldTransform);
				DirectX::XMMATRIX boneTransform = inverseTransform * worldTransform;
				DirectX::XMStoreFloat4x4(&mCbBoneBuffer->data.boneTransForm[j], boneTransform);
			}
		}
		else
		{
			mCbBoneBuffer->data.boneTransForm[0] = nodes[boneData.nodeIndex].worldTransform;
		}
		//GPUにデータを送信
		mCbCreateBuffer->Activate(context, 1, false, false, false, true);
		mCbBoneBuffer->Activate(context, 0, false, false, false, true);
		mesh.mVertex->Activate(context, 0, false, true);
		mesh.mIndex->Activate(context, 1, false, true);
		//生成
		context->Dispatch(mesh.mMeshSize, 1, 1);
		//解放
		mCbCreateBuffer->DeActivate(context);
		mCbBoneBuffer->DeActivate(context);
		mesh.mVertex->DeActivate(context);
		mesh.mIndex->DeActivate(context);

	}

}
/*************************パーティクルの更新関数****************************/

void RespondParticle::Move(ID3D11DeviceContext* context, std::shared_ptr<PlayerAI>player, float elapsd_time)
{
	if (!mCreateFlag)return;
	context->CSSetShader(mCSShader.Get(), nullptr, 0);
	mCbUpdateBuffer->data.elapsdTime = elapsd_time;
	mCbUpdateBuffer->Activate(context, 3, false, false, false, true);

	context->Dispatch(mMaxParticle / 100, 1, 1);
	mCbUpdateBuffer->DeActivate(context);
	//パーティクル数の更新
	context->CSSetShader(mStartCSShader.Get(), nullptr, 0);
	context->Dispatch(1, 1, 1);

	//カウントを取得
	D3D11_MAPPED_SUBRESOURCE ms;
	context->Map(mParticleCount->GetBuffer(), NULL, D3D11_MAP_READ, NULL, &ms);
	ParticleCount* particleCount = (ParticleCount*)ms.pData;
	mRenderCount = particleCount->aliveParticleCount;
	context->Unmap(mParticleCount->GetBuffer(), NULL);

	//生成してからの時間を元にカメラなどの位置を変更
	mTimer += elapsd_time;

	if (mTimer - mEditorData.expansionTime >= mEditorData.respondTime * 0.5f)
	{
		pCameraManager->GetCameraOperation()->GetPlayCamera()->SetPlayerPosition(VECTOR3F(0, 10, 0));
	}
	if (mRenderCount == 0)
	{
		player->Respond();
		mCreateFlag = false;
		mTimer = 0.0f;
	}
}

/*****************************************************/
//　　　　　　　　　　描画関数
/*****************************************************/

void RespondParticle::Render(ID3D11DeviceContext* context)
{
	//GPU側にデータを送る
	mShader->Activate(context);

	u_int stride = sizeof(RenderParticle);
	u_int offset = 0;
	ID3D11Buffer* vertex = mParticleRender->GetBuffer();
	ID3D11Buffer* index = mParticleIndexs[mIndexNum]->GetBuffer();
	context->IASetVertexBuffers(0, 1, &vertex, &stride, &offset);
	context->IASetIndexBuffer(index, DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	//描画
	context->DrawIndexed(mRenderCount, 0, 0);
	//送ったデータを元に戻す
	mShader->Deactivate(context);

}

