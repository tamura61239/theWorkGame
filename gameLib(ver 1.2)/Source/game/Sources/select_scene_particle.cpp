#include "select_scene_particle.h"
#include"misc.h"
#include"shader.h"
#include"camera_manager.h"
#include"texture.h"
#include"file_function.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif

/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�������֐�(�R���X�g���N�^)
/*****************************************************/
SelectSceneParticle::SelectSceneParticle(ID3D11Device* device) :mIndexCount(0), mRenderCount(0)
{
	HRESULT hr;

	mMaxParticle = 60000;
	memset(&mEditorData, 0, sizeof(mEditorData));

	{
		//�p�[�e�B�N���̃o�b�t�@
		std::vector<Particle>particles;
		particles.resize(static_cast<size_t>(mMaxParticle));
		mParticle = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, particles, true);
		//�`��p�o�b�t�@
		std::vector<RenderParticle>renderParticles;
		renderParticles.resize(static_cast<size_t>(mMaxParticle));
		mParticleRender = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS, renderParticles, true);
		//index�o�b�t�@
		std::vector<UINT>indices;
		indices.resize(static_cast<size_t>(mMaxParticle));
		for (auto& index : mParticleIndices)
		{
			index = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS, indices, true);
		}
		//deleteIndex�o�b�t�@
		for (int i = 0; i < mMaxParticle; i++)
		{
			indices[i] = i;
		}
		mParticleDeleteIndex = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, indices, true);
		//�p�[�e�B�N���̃J�E���g�o�b�t�@
		ParticleCount particleCount;
		memset(&particleCount, 0, sizeof(particleCount));
		particleCount.deActiveParticleCount = static_cast<UINT>(mMaxParticle);
		mParticleCount = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS, particleCount, true, false, D3D11_CPU_ACCESS_READ);
	}

	//�萔�o�b�t�@
	mCbCreate = std::make_unique<ConstantBuffer<CbCreate>>(device);
	mCbUpdate = std::make_unique<ConstantBuffer<CbUpdate>>(device);
	//�`��p�̃e�N�X�`���̐���
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

	//�R���s���[�g�V�F�[�_�[�̐���
	hr = CreateCSFromCso(device, "Data/shader/select_scene_create_particle_cs.cso", mCreateShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = CreateCSFromCso(device, "Data/shader/select_scene_particle_cs.cso", mCSShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = CreateCSFromCso(device, "Data/shader/particle_count_cs.cso", mCSEndShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//�`��p�̃V�F�[�_�[�̐���
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"ANGLE",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"VELOCITY",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"SCALE",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
	};
	mShader = std::make_unique<DrowShader>(device, "Data/shader/particle_render_vs.cso", "Data/shader/particle_render_billboard_gs.cso", "Data/shader/particle_render_text_ps.cso", inputElementDesc, ARRAYSIZE(inputElementDesc));
	//�f�[�^�̐ݒ�
	mEditorData.angleMovement = VECTOR3F(3.14f / 2.f, 3.14f, 0);
	mEditorData.color = VECTOR4F(0, 1, 0.5f, 1);
	mEditorData.range = 200;
	mEditorData.scope = VECTOR3F(1, 1, 0.3f);
	mEditorData.speed = 200;
	mEditorData.endPosition = VECTOR3F(0, 0, 1000);
	mEditorData.defVelocity = VECTOR3F(0, 0, 1);
	mEditorData.sinLeng = 10;
	FileFunction::Load(mEditorData, "Data/file/selete_scene_particle_data.bin", "rb");
}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�G�f�B�^�֐�
/*****************************************************/
void SelectSceneParticle::Editor()
{
#ifdef USE_IMGUI
	ImGui::Begin("select scene particle");
	//�p�����[�^�[�𒲐�����
	ImGui::InputFloat("speed", &mEditorData.speed, 0.5f);
	ImGui::InputFloat("range", &mEditorData.range, 1);
	float* color[4] = { &mEditorData.color.x,&mEditorData.color.y,&mEditorData.color.z,&mEditorData.color.w };
	ImGui::ColorEdit4("def color", *color);
	float* scope[3] = { &mEditorData.scope.x,&mEditorData.scope.y,&mEditorData.scope.z };
	ImGui::SliderFloat3("scope", *scope, 0, 1);
	float* angleMovement[3] = { &mEditorData.angleMovement.x,&mEditorData.angleMovement.y ,&mEditorData.angleMovement.z };
	ImGui::SliderFloat3("def angleMovement", *angleMovement, -3.14f, 3.14f);
	ImGui::InputFloat("sinLeng", &mEditorData.sinLeng, 0.1f);
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
	//�Z�[�u
	if (ImGui::Button("save"))
	{
		FileFunction::Save(mEditorData, "Data/file/selete_scene_particle_data.bin", "wb");

	}
	ImGui::Text("%f", newIndex);
	ImGui::End();
#endif

}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�X�V�֐�
/*****************************************************/

void SelectSceneParticle::Update(float elapsdTime, ID3D11DeviceContext* context)
{
	//GPU���Ƀp�[�e�B�N���Ƀf�[�^�𑗂�
	mParticle->Activate(context, 0, true);
	mParticleCount->Activate(context, 1, true);
	mParticleRender->Activate(context, 2, true);
	mParticleIndices[mIndexCount]->Activate(context, 3, true);
	mParticleIndices[1 - mIndexCount]->Activate(context, 4, true);
	mParticleDeleteIndex->Activate(context, 5, true);
	//�萔�o�b�t�@�̃f�[�^�X�V
	mCbCreate->data.angleMovement = mEditorData.angleMovement;
	mCbCreate->data.color = mEditorData.color;
	mCbCreate->data.range = mEditorData.range;
	mCbCreate->data.scope = mEditorData.scope;
	mCbCreate->data.speed = mEditorData.speed;
	mCbCreate->data.sinLeng = mEditorData.sinLeng;
	mCbUpdate->data.endPosition = mEditorData.endPosition;
	mCbUpdate->data.defVelocity = mEditorData.defVelocity;

	mIndexCount++;
	if (mIndexCount >= 2)mIndexCount = 0;
	//�������鐔�̍X�V
	newIndex += elapsdTime * 2000;
	//�p�[�e�B�N���̐���
	if (newIndex >= 1)
	{
		mCbCreate->data.eye = pCameraManager->GetCamera()->GetEye();
		mCbCreate->data.eye.y = 0;
		//�V�F�[�_�[�̐ݒ�
		context->CSSetShader(mCreateShader.Get(), nullptr, 0);
		//�萔�o�b�t�@��GPU���ɑ���
		mCbCreate->Activate(context, 0, false, false, false, true);
		//�����_�ȉ��؂�̂�
		UINT count = static_cast<UINT>(newIndex);
		//�X�V
		context->Dispatch(count, 1, 1);
		//�������f�[�^�����ɖ߂�
		mCbCreate->DeActivate(context);
		//�������������������Ƃ�
		newIndex -= static_cast<float>(count);
	}
	//�p�[�e�B�N���̍X�V
	context->CSSetShader(mCSShader.Get(), nullptr, 0);
	mCbUpdate->data.elapsdTime = elapsdTime;
	mCbUpdate->Activate(context, 1, false, false, false, true);
	context->Dispatch(static_cast<UINT>(mMaxParticle * 0.01f), 1, 1);
	mCbUpdate->DeActivate(context);
	//�p�[�e�B�N���J�E���g�̍X�V
	context->CSSetShader(mCSEndShader.Get(), nullptr, 0);
	context->Dispatch(1, 1, 1);
	//�J�E���g�̃o�b�t�@���琶���Ă镪�̃J�E���g���擾
	D3D11_MAPPED_SUBRESOURCE ms;
	context->Map(mParticleCount->GetBuffer(), NULL, D3D11_MAP_READ, NULL, &ms);
	ParticleCount* particleCount = (ParticleCount*)ms.pData;
	mRenderCount = particleCount->aliveParticleCount;
	context->Unmap(mParticleCount->GetBuffer(), NULL);
	//�V�F�[�_�[�̉���
	context->CSSetShader(nullptr, nullptr, 0);
	//GPU���ɑ������f�[�^�����ɖ߂�
	mParticle->DeActivate(context);
	mParticleCount->DeActivate(context);
	mParticleRender->DeActivate(context);
	mParticleIndices[mIndexCount]->DeActivate(context);
	mParticleIndices[1 - mIndexCount]->DeActivate(context);
	mParticleDeleteIndex->DeActivate(context);

}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�`��֐�
/*****************************************************/

void SelectSceneParticle::Render(ID3D11DeviceContext* context)
{
	//�V�F�[�_�[�̐ݒ�
	mShader->Activate(context);
	//GPU���Ƀf�[�^�𑗂�
	context->PSSetShaderResources(0, 1, mParticleSRV[mEditorData.textureType].GetAddressOf());
	u_int stride = sizeof(RenderParticle);
	u_int offset = 0;
	ID3D11Buffer* vertex = mParticleRender->GetBuffer();
	ID3D11Buffer* index = mParticleIndices[mIndexCount]->GetBuffer();
	context->IASetVertexBuffers(0, 1, &vertex, &stride, &offset);
	context->IASetIndexBuffer(index, DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	//�`��
	context->DrawIndexed(mRenderCount, 0, 0);
	//�������f�[�^�����ɖ߂�
	mShader->Deactivate(context);
	offset = 0;
	vertex = nullptr;
	index = nullptr;
	context->IASetVertexBuffers(0, 1, &vertex, &stride, &offset);
	context->IASetIndexBuffer(index, DXGI_FORMAT_R32_UINT, 0);
	ID3D11ShaderResourceView* srv = nullptr;
	context->PSSetShaderResources(0, 1, &srv);

}

