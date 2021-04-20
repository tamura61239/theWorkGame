#include "title_texture_particle.h"
#include"texture.h"
#include"misc.h"
#include"shader.h"
#include"framework.h"
#include"camera_manager.h"
#include <codecvt>
#include <locale>
#include"file_function.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif

/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�������֐�(�R���X�g���N�^)
/*****************************************************/

TitleTextureParticle::TitleTextureParticle(ID3D11Device* device) :mFullCreateFlag(false), mParticleFlag(false), mMaxParticle(1920 * 1080), mTestFlag(false)
, mChangeMaxParticle(0), mMaxTexture(0), mTextureFlag(true)
{
	HRESULT hr;
	{
		//�p�[�e�B�N���̃o�b�t�@
		std::vector<Particle>particles;
		particles.resize(mMaxParticle);
		mParticle = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, particles, true);
		//�`��p�o�b�t�@
		std::vector<RenderParticle>renderParticles;
		renderParticles.resize(mMaxParticle);
		mParticleRender = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS, renderParticles, true);
	}
	//�萔�o�b�t�@����
	mCbCreate = std::make_unique<ConstantBuffer<CbCreate>>(device);
	mCbUpdate = std::make_unique<ConstantBuffer<CbUpdate>>(device);
	//�R���s���[�g�V�F�[�_�[�̐���
	mCreateCSShader.resize(2);
	CreateCSFromCso(device, "Data/shader/titile_texture_change_creat_cs.cso", mCreateCSShader[0].GetAddressOf());
	CreateCSFromCso(device, "Data/shader/title_texture_scene_change_cs.cso", mCSShader.GetAddressOf());
	//�`��p�e�N�X�`���̐���
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
	//�G�f�B�^�f�[�^�̏�����
	mEditorData.scale = 0.00015f;
	mEditorData.screenSplit = 10;
	//�t�@�C������f�[�^�����[�h
	Load();
}
/************************wstring�^��string�^�ɕϊ�����֐�****************/
using convert_t = std::codecvt_utf8<wchar_t>;
std::wstring_convert<convert_t, wchar_t> strconverter;

std::string to_string(std::wstring wstr)
{
	return strconverter.to_bytes(wstr);
}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�G�f�B�^�֐�
/*****************************************************/

void TitleTextureParticle::Editor()
{
#ifdef USE_IMGUI
	ImGui::Begin("title texture particle");
	ImGui::Checkbox("create", &mFullCreateFlag);
	if (mParticleFlag || mTestFlag)ImGui::Checkbox("move", &mParticleFlag);

	ImGui::Checkbox("test", &mTestFlag);
	ImGui::Checkbox("drow texture", &mTextureFlag);
	ImGui::InputFloat("speed", &mEditorData.speed, 0.1f);
	ImGui::InputFloat("scale", &mEditorData.scale, 0.1f);
	ImGui::InputFloat("screen split", &mEditorData.screenSplit, 0.1f);
	static int num = 0;
	for (int i = 0; i < static_cast<int>(mTextures.size()); i++)
	{
		auto& texture = mTextures[i];
		std::string name = to_string(texture.data.mTextureName);
		ImGui::RadioButton(name.c_str(), &num, i);
	}
	auto& board = boards[num];
	float* position[3] = { &board.position.x,&board.position.y,&board.position.z };
	ImGui::DragFloat3("position", *position, 1);
	ImGui::InputFloat("board scale x", &board.scale.x, 1);
	ImGui::InputFloat("board scale y", &board.scale.y, 1);

	ImVec2 size = ImVec2(75, 75);
	for (UINT i = 0; i < static_cast<UINT>(mParticleSRV.size()); i++)
	{
		if (ImGui::ImageButton(mParticleSRV[i].Get(), size))
		{
			mEditorData.textureType = i;
		}
		if (i % 4 < 3&&i< static_cast<UINT>(mParticleSRV.size()-1))ImGui::SameLine();
	}
	ImGui::Text(u8"���̃e�N�X�`��");
	size = ImVec2(150, 150);
	ImGui::Image(mParticleSRV[mEditorData.textureType].Get(), size);
	if (ImGui::Button("save"))
	{
		Save();
	}
	ImGui::Text("particle:%d", mMoveParticle);
	ImGui::End();
#endif
}
/***************************�p�[�e�B�N���𐶐�����̂Ɏg���e�N�X�`���̓ǂݍ��݊֐�********************************/
void TitleTextureParticle::LoadTexture(ID3D11Device* device, std::wstring name, const VECTOR2F& leftTop, const VECTOR2F& size, const VECTOR2F& uv, const VECTOR2F& uvSize)
{
	mTextures.emplace_back();
	auto& textute = mTextures.back();
	HRESULT hr = LoadTextureFromFile(device, name.c_str(), textute.mSRV.GetAddressOf());
	textute.data.mLeftTop = leftTop;
	textute.data.mSize = size;
	textute.data.mUVLeftTop = uv;
	textute.data.mUVSize = uvSize;
	textute.data.mTextureName = name;
	if (boards.size() < mTextures.size())
	{
		boards.emplace_back();
		boards.back().position = VECTOR3F(0, 0, 0);
		boards.back().scale = VECTOR3F(0, 0, 0);
	}
	mMaxTexture += static_cast<UINT>(uvSize.x);
}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�X�V�֐�
/*****************************************************/
/**************************�p�[�e�B�N���̍X�V***************************/
void TitleTextureParticle::Update(float elapsdTime, ID3D11DeviceContext* context)
{
	//�p�[�e�B�N���̃f�[�^��GPU���ɑ���
	mParticle->Activate(context, 0, true);
	SceneChangeUpdate(elapsdTime, context);
	if (mMoveParticle <= 0)
	{
		mParticle->DeActivate(context);
		return;
	}
	if (mTestFlag && !mParticleFlag)elapsdTime = 0;
	//�p�[�e�B�N���̃f�[�^��GPU���ɑ���
	mParticleRender->Activate(context, 2, true);

	//�V�F�[�_�[��ݒ�
	context->CSSetShader(mCSShader.Get(), nullptr, 0);
	//�萔�o�b�t�@�̃f�[�^���X�V
	mCbUpdate->data.elapsdTime = elapsdTime;
	mCbUpdate->data.scale = mEditorData.scale;
	mCbUpdate->data.speed = mEditorData.speed;
	//�萔�o�b�t�@��GPU���ɑ���
	mCbUpdate->Activate(context, 1, false, false, false, true);
	//�X�V
	context->Dispatch(static_cast<UINT>(mMoveParticle / 100), 1, 1);

	//GPU���ɑ������f�[�^�����ɖ߂�
	mParticle->DeActivate(context);
	mParticleRender->DeActivate(context);
	mCbUpdate->DeActivate(context);
	context->CSSetShader(nullptr, nullptr, 0);

}

/************************�V�[�����ύX�����Ƃ��̃p�[�e�B�N������*****************************/
void TitleTextureParticle::SceneChangeUpdate(float elapsdTime, ID3D11DeviceContext* context)
{
	//�e�X�g�v���C�t���O��ON�ɂȂ��Ă邩�����t���O��ON�ɂȂ��Ă���Ƃ�
	if (mFullCreateFlag || mTestFlag)
	{
		//�ǂ̃t���O��ON�ɂȂ��Ă���̂����ׂĂ��̌��ʂŏ�����ς��Ă���
		if (mFullCreateFlag)mParticleFlag = true;
		if (mTestFlag)mChangeMaxParticle = 0;
		mFullCreateFlag = false;
		//�V�F�[�_�[��ݒ�
		context->CSSetShader(mCreateCSShader[0].Get(), nullptr, 0);
		//view�s����擾
		FLOAT4X4 view = pCameraManager->GetCamera()->GetView();
		view._41 = view._42 = view._43 = 0.0f;
		view._44 = 1.0f;
		int particleCount = 0;
		//�e�N�X�`��(�|��)�̕������p�[�e�B�N���̐�������������
		for (int i = 0; i < static_cast<int>(mTextures.size()); i++)
		{
			auto& texture = mTextures.at(i);
			auto& board = boards.at(i);
			//�e�N�X�`����GPU���ɓn��
			context->CSSetShaderResources(0, 1, texture.mSRV.GetAddressOf());
			//�萔�o�b�t�@�̃f�[�^���X�V
			mCbCreate->data.uvSize = texture.data.mUVSize;
			mCbCreate->data.screenSplit = mEditorData.screenSplit;
			mCbCreate->data.startIndex = particleCount;
			float aspect = texture.data.mUVSize.x / texture.data.mUVSize.y;
			DirectX::XMMATRIX W;
			{
				DirectX::XMMATRIX S, T;
				S = DirectX::XMMatrixScaling(board.scale.x * aspect, board.scale.y, 0);
				T = DirectX::XMMatrixTranslation(board.position.x, board.position.y, board.position.z);
				W = S * T;

			}

			DirectX::XMStoreFloat4x4(&mCbCreate->data.world, DirectX::XMLoadFloat4x4(&view) * W);
			//�萔�o�b�t�@��GPU���ɑ���
			mCbCreate->Activate(context, 0, false, false, false, true);
			int newParticle = static_cast<int>(texture.data.mUVSize.x / mCbCreate->data.screenSplit * texture.data.mUVSize.y / mCbCreate->data.screenSplit);
			//�p�[�e�B�N���̐��𑝂₷
			particleCount += newParticle;
			//����
			context->Dispatch(newParticle, 1, 1);
			//�萔�o�b�t�@�����ɖ߂�
			mCbCreate->DeActivate(context);

		}
		//�������������p�[�e�B�N�����ɒǉ�
		mChangeMaxParticle += particleCount;
	}
	else if (!mParticleFlag)
	{
		mChangeMaxParticle = 0;
	}
	mMoveParticle = mChangeMaxParticle;

}


/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�`��֐�
/*****************************************************/
void TitleTextureParticle::Render(ID3D11DeviceContext* context)
{
	if (mMoveParticle <= 0)return;
	//�V�F�[�_�[��ݒ�
	mShader->Activate(context);
	//�f�[�^��GPU���ɑ���
	ID3D11Buffer* buffer = mParticleRender->GetBuffer();
	u_int stride = sizeof(RenderParticle);
	u_int offset = 0;
	context->PSSetShaderResources(0, 1, mParticleSRV[mEditorData.textureType].GetAddressOf());
	context->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	//�`��
	context->Draw(mMoveParticle, 0);
	//�V�F�[�_�[�����ɖ߂�
	mShader->Deactivate(context);
	//GPU���ɑ������f�[�^�����ɖ߂�
	ID3D11ShaderResourceView* srv = nullptr;
	context->PSSetShaderResources(0, 1, &srv);
	buffer = nullptr;
	stride = 0;
	context->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);

}


void TitleTextureParticle::Load()
{
	FILE* fp;
	long size = 0;
	FileFunction::Load(mEditorData, "Data/file/title_texture_data1.bin", "rb");
	if (fopen_s(&fp, "Data/file/title_texture_data2.bin", "rb") == 0)
	{
		fseek(fp, 0, SEEK_END);
		size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		int s = size / sizeof(Board);
		boards.resize(s);
		fread(boards.data(), size, 1, fp);
		fclose(fp);
	}

}

void TitleTextureParticle::Save()
{
	FILE* fp;
	FileFunction::Save(mEditorData, "Data/file/title_texture_data1.bin", "wb");

	fopen_s(&fp, "Data/file/title_texture_data2.bin", "wb");
	fwrite(&boards[0], sizeof(Board) * boards.size(), 1, fp);
	fclose(fp);

}

