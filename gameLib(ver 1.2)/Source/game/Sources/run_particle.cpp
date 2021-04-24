#include "run_particle.h"
#include"misc.h"
#include"shader.h"
#include"texture.h"
#include"file_function.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�������֐�(�R���X�g���N�^)
/*****************************************************/

RunParticles::RunParticles(ID3D11Device* device, std::shared_ptr<PlayerAI>player) :mMaxParticle(0), mTimer(0), mIndexNum(0), mRenderCount(0), mTestFlag(false)
{
	memset(&mEditorData, 0, sizeof(mEditorData));
	memset(&mEditorData.mColor, 1, sizeof(mEditorData.mColor));
	mEditorData.mCreateCount = 3;
	/************�萔�o�b�t�@�쐬**************/
	mCbBoneBuffer = std::make_unique<ConstantBuffer<CbBone>>(device);
	mCbCreateBuffer = std::make_unique<ConstantBuffer<CbCreate>>(device);
	mCbUpdateBuffer = std::make_unique<ConstantBuffer<CbUpdate>>(device);
	int totalIndex = 0;
	HRESULT hr;
	mPlayer = player;
	/**************************���b�V���̃o�b�t�@��SRV*****************************/
	for (auto& mesh : player->GetCharacter()->GetModel()->GetModelResource()->GetMeshDatas())
	{
		mMeshs.emplace_back();
		mMeshs.back().mIndex = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, mesh.indices, false, true);
		mMeshs.back().mVertex = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, mesh.vertices, false, true);
		mMeshs.back().mMwshSize = mesh.indices.size() / 3;
		totalIndex += mesh.indices.size();
		
	}
	//�p�[�e�B�N���̍ő吔���v�Z
	int count = totalIndex / 10000;
	mMaxParticle = 100000 * (count + 1);
	{
		//�p�[�e�B�N���̃o�b�t�@
		std::vector<Particle>particles;
		particles.resize(mMaxParticle);
		mParticle = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, particles, true);
		//index�o�b�t�@
		std::vector<UINT>indices;
		indices.resize(mMaxParticle);
		for (int i = 0; i < 2; i++)mParticleIndexs[i] = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS, indices, true);
		//deleteindex�o�b�t�@
		for (int i = 0; i < mMaxParticle; i++)
		{
			indices[i] = i;
		}
		mParticleDeleteIndex = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, indices, true);
		//�`��p�o�b�t�@
		std::vector<RenderParticle>renderParticles;
		renderParticles.resize(mMaxParticle);
		mParticleRender = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS, renderParticles, true);
		//�J�E���g�o�b�t�@
		ParticleCount particleCount;
		memset(&particleCount, 0, sizeof(particleCount));
		particleCount.deActiveParticleCount = mMaxParticle;
		mParticleCount = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS, particleCount, true, false, D3D11_CPU_ACCESS_READ);
	}
	//�R���s���[�g�V�F�[�_�[�̐���
	hr = CreateCSFromCso(device, "Data/shader/run_particle_create_cs.cso", mCreateCSShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = CreateCSFromCso(device, "Data/shader/run_particle_cs.cso", mCSShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = CreateCSFromCso(device, "Data/shader/particle_count_cs.cso", mStartCSShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//�`��p�f�[�^�̐���
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"ANGLE",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"VELOCITY",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"SCALE",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
	};
	mShader = std::make_unique<DrowShader>(device, "Data/shader/particle_render_vs.cso", "Data/shader/particle_render_billboard_gs.cso", "Data/shader/particle_render_text_ps.cso", inputElementDesc, ARRAYSIZE(inputElementDesc));
	//�`��p�e�N�X�`���̐���
	wchar_t* names[] =
	{
		L"Data/image/��.png",
		L"",
		L"Data/image/����1.png",
		L"Data/image/����2.png",
		L"Data/image/����3.png",
		L"Data/image/����4.png",
		L"Data/image/����5.png",
		L"Data/image/����6.png",
		L"Data/image/����7.png",
		L"Data/image/����8.png",
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
	//�t�@�C������f�[�^�̃��[�h
	FileFunction::Load(mEditorData, "Data/file/run_particle.bin", "rb");

}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�G�f�B�^�֐�
/*****************************************************/

void RunParticles::Editor()
{
#ifdef USE_IMGUI
	ImGui::Begin("run particle");
	//�p�����[�^�[�𒲐�����
	ImGui::ColorEdit4("color", mEditorData.mColor);
	ImGui::SliderInt(u8"1���b�V������o��p�[�e�B�N����", &mEditorData.mCreateCount, 0, 10);
	ImGui::InputFloat(u8"�p�[�e�B�N�����o���Ԋu", &mEditorData.mCreateTime, 0.1f);
	ImGui::InputFloat(u8"���x", &mEditorData.speed, 0.1f);
	ImGui::InputFloat("life", &mEditorData.life, 0.1f);
	ImGui::Checkbox("test", &mTestFlag);
	//�`��Ɏg���e�N�X�`����I������
	ImVec2 size = ImVec2(75, 75);
	for (UINT i = 0; i < static_cast<UINT>(mParticleSRV.size()); i++)
	{
		if (ImGui::ImageButton(mParticleSRV[i].Get(), size))
		{
			mEditorData.textureType = i;
		}
		if (i % 4 < 3 && i < static_cast<UINT>(mParticleSRV.size() - 1))ImGui::SameLine();
	}
	//���I�����Ă�e�N�X�`���̕\��
	ImGui::Text(u8"���̃e�N�X�`��");
	size = ImVec2(150, 150);
	ImGui::Image(mParticleSRV[mEditorData.textureType].Get(), size);

	ImGui::Text("%d", mRenderCount);
	//�Z�[�u
	if (ImGui::Button("save"))
	{
		FileFunction::Save(mEditorData, "Data/file/run_particle.bin", "wb");
	}
	ImGui::End();
#endif
}

/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�X�V�֐�
/*****************************************************/

void RunParticles::Update(ID3D11DeviceContext* context, float elapsd_time)
{
	//UAV��GPU�ɓn��
	mParticle->Activate(context, 0, true);
	mParticleCount->Activate(context, 1, true);
	mParticleRender->Activate(context, 2, true);
	mParticleIndexs[mIndexNum]->Activate(context, 3, true);
	mParticleIndexs[1 - mIndexNum]->Activate(context, 4, true);
	mParticleDeleteIndex->Activate(context, 5, true);
	//�p�[�e�B�N������
	if (mPlayer.lock()->GetPlayFlag() || mTestFlag)mTimer += elapsd_time;
	mIndexNum++;
	if (mIndexNum >= 2)mIndexNum = 0;
	//���Ԃ��������邽�߂̎���(��������Ԋu)�ȏ�ɂȂ����琶��
	if (mTimer >= mEditorData.mCreateTime)
	{
		context->CSSetShader(mCreateCSShader.Get(), nullptr, 0);
		const ModelResource* resouce = mPlayer.lock()->GetCharacter()->GetModel()->GetModelResource();
		const std::vector<Model::Node>& nodes = mPlayer.lock()->GetCharacter()->GetModel()->GetNodes();
		mCbCreateBuffer->data.color = 0;
		mCbCreateBuffer->data.color |= (static_cast<UINT>(mEditorData.mColor[0] * 255) & 0x00FFFFFF) << 24;
		mCbCreateBuffer->data.color |= (static_cast<UINT>(mEditorData.mColor[1] * 255) & 0x00FFFFFF) << 16;
		mCbCreateBuffer->data.color |= (static_cast<UINT>(mEditorData.mColor[2] * 255) & 0x00FFFFFF) << 8;
		mCbCreateBuffer->data.color |= (static_cast<UINT>(mEditorData.mColor[3] * 255) & 0x00FFFFFF) << 0;
		if (mEditorData.mCreateCount > 0)
		{
			mCbCreateBuffer->data.speed = mEditorData.speed;
			mCbCreateBuffer->data.life = mEditorData.life;
			//���b�V���f�[�^����p�[�e�B�N���𐶐�
			for (int i = 0; i < static_cast<int>(mMeshs.size()); i++)
			{
				const auto& mesh = mMeshs[i];
				const auto& boneData = resouce->GetMeshes()[i];
				if (boneData.nodeIndices.size() > 0)
				{
					//�{�[���̃��[���h�s��̌v�Z
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
				//GPU�Ƀf�[�^��ݒ�
				mCbCreateBuffer->data.indexCount = mesh.mMwshSize;
				mCbCreateBuffer->Activate(context, 1, false, false, false, true);
				mCbBoneBuffer->Activate(context, 0, false, false, false, true);
				mesh.mVertex->Activate(context, 0, false, true);
				mesh.mIndex->Activate(context, 1, false, true);
				//����
				context->Dispatch(mesh.mMwshSize * mEditorData.mCreateCount, 1, 1);
				//���
				mCbCreateBuffer->DeActivate(context);
				mCbBoneBuffer->DeActivate(context);
				mesh.mVertex->DeActivate(context);
				mesh.mIndex->DeActivate(context);

			}
			mTimer = 0;
		}
	}
	//�p�[�e�B�N���̍X�V
	context->CSSetShader(mCSShader.Get(), nullptr, 0);

	mCbUpdateBuffer->data.elapsdTime = elapsd_time;
	mCbUpdateBuffer->Activate(context, 2, false, false, false, true);

	context->Dispatch(mMaxParticle / 100, 1, 1);
	mCbUpdateBuffer->DeActivate(context);
	//�p�[�e�B�N�����̍X�V
	context->CSSetShader(mStartCSShader.Get(), nullptr, 0);
	context->Dispatch(1, 1, 1);

	mParticle->DeActivate(context);
	mParticleCount->DeActivate(context);
	mParticleRender->DeActivate(context);
	mParticleIndexs[mIndexNum]->DeActivate(context);
	mParticleIndexs[1 - mIndexNum]->DeActivate(context);
	mParticleDeleteIndex->DeActivate(context);

	//�J�E���g���擾
	D3D11_MAPPED_SUBRESOURCE ms;
	context->Map(mParticleCount->GetBuffer(), NULL, D3D11_MAP_READ, NULL, &ms);
	ParticleCount* particleCount = (ParticleCount*)ms.pData;
	mRenderCount = particleCount->aliveParticleCount;
	context->Unmap(mParticleCount->GetBuffer(), NULL);
	//GPU�̃f�[�^�����
	context->CSSetShader(nullptr, nullptr, 0);

}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�`��֐�
/*****************************************************/

void RunParticles::Render(ID3D11DeviceContext* context)
{
	//GPU���Ƀf�[�^�𑗂�
	mShader->Activate(context);
	context->PSSetShaderResources(0, 1, mParticleSRV[mEditorData.textureType].GetAddressOf());

	u_int stride = sizeof(RenderParticle);
	u_int offset = 0;
	ID3D11Buffer* vertex = mParticleRender->GetBuffer();
	ID3D11Buffer* index = mParticleIndexs[mIndexNum]->GetBuffer();
	context->IASetVertexBuffers(0, 1, &vertex, &stride, &offset);
	context->IASetIndexBuffer(index, DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST);
	//�`��
	context->DrawIndexed(mRenderCount, 0, 0);
	//�������f�[�^�����ɖ߂�
	mShader->Deactivate(context);
	ID3D11ShaderResourceView* srv = nullptr;
	context->PSSetShaderResources(0, 1, &srv);
}



