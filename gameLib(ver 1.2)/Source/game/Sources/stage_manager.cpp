#include "stage_manager.h"
#include"framework.h"
#include"camera_manager.h"
#include"key_board.h"
#include"gpu_particle_manager.h"
#include"hit_area_render.h"
#include"file_function.h"
int StageManager::mMaxStage = 0;
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�������֐�(�R���X�g���N�^)
/*****************************************************/
StageManager::StageManager(ID3D11Device* device, int width, int height) :mStageNo(3), mWidth(static_cast<float>(width)), mHeight(static_cast<float>(height)), dragObjNumber(-1)
{
	//�X�e�[�W�ɂ���I�u�W�F�N�g����ނ��Ƃɐ���
	mMeshs.push_back(std::make_shared<StaticMesh>(device, "Data/FBX/000_cube.fbx"));
	mMeshs.push_back(std::make_shared<StaticMesh>(device, "Data/FBX/jumpstand.fbx"));
	mMeshs.push_back(std::make_shared<StaticMesh>(device, "Data/FBX/go-ru.fbx"));
	//�`��p�f�[�^�̐���
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
	//�X�e�[�W�G�f�B�^�̐���
	mEditor = std::make_unique<StageEditor>(device, 1920, 1080);
}
/***********************�X�e�[�W�̍ő吔�𒲂ׂ�*******************/
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
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�t�@�C������֐�
/*****************************************************/
/****************************���[�h**************************/
void StageManager::Load()
{
	std::string fileName = { "Data/file/stage" };
	fileName += std::to_string(mStageNo) + ".bin";
	std::vector<StageData>data;
	FileFunction::LoadArray(data, fileName.c_str(), "rb");

	for (auto& d : data)
	{
		mStageObjs.push_back(std::make_shared<StageObj>(mMeshs[d.mObjType]));
		mStageObjs.back()->SetStageData(d);
	}

}
/******************************�Z�[�u**************************/
void StageManager::Save()
{
	std::string fileName = { "Data/file/stage" };
	fileName += std::to_string(mStageNo) + ".bin";
	std::vector<StageData>data;
	for (auto& obj : mStageObjs)
	{
		data.push_back(obj->GetStageData());
	}
	FileFunction::SaveArray(&data[0], data.size(), fileName.c_str(), "wb");
}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�G�f�B�^�֐�
/*****************************************************/
void StageManager::Editor()
{
#ifdef USE_IMGUI
	//�G�f�B�^�N���X�̍X�V
	mEditor->Editor(mStageObjs);
	//�t�@�C�����������ꍇ�͂����ł���
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
	//�G�f�B�^�̏�Ԃ̃��Z�b�g
	mEditor->ClearFileState();
	//��������ꍇ�͂����ł���
	if (mEditor->GetCreateFlag())
	{
		StageData data = mEditor->GetCreateData();
		mStageObjs.push_back(std::make_shared<StageObj>(mMeshs[data.mObjType]));
		mStageObjs.back()->SetStageData(data);
		mEditor->ClearCreateData();
	}
	//�����ꍇ�̂����ł���
	int deleteNo = mEditor->GetDeleteNo();
	if (deleteNo != -1)
	{
		mStageObjs.erase(mStageObjs.begin() + deleteNo);
	}
#endif
}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�X�V�֐�
/*****************************************************/
void StageManager::Update(float elapsd_time, const int stageState)
{
	mReds.clear();
	mBlues.clear();
	//�X�e�[�W�I�u�W�F�N�g�̓����蔻��𓖂��蔻���`�悷��N���X�ɓn��
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
		//�F���Ƃɕ`��p�̔z��ɃZ�b�g����
		if (stage->GetStageData().mColorType == stageState)
		{
			mReds.push_back(stage);
		}
		else
		{
			mBlues.push_back(stage);
		}
	}
}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�`��֐�
/*****************************************************/
/*************************�ʏ�`��****************************/
void StageManager::Render(ID3D11DeviceContext* context)
{
	VECTOR4F color[2] = { mReds[0]->GetColor(),mBlues[0]->GetColor() };
	//�ԐF�̕`��
	for (auto& red : mReds)
	{
		mRender->Render(context, red->GetMesh(), red->GetWorld(), red->GetColor());
	}
	//�F�̕`��
	for (auto& blur : mBlues)
	{
		mRender->Render(context, blur->GetMesh(), blur->GetWorld(), blur->GetColor());
	}
	//�V������������I�u�W�F�N�g�����ŕ`��
	mEditor->EditorCreateObjImageRender(context, mMeshs.at(mEditor->GetCreateData().mObjType).get(), mRender.get(), color[mEditor->GetCreateData().mColorType]);
}
/*************************�ʏ�`��(�V�F�[�_�[�������Ă���)****************************/

void StageManager::Render(ID3D11DeviceContext* context, DrowShader* srv)
{
	VECTOR4F color[2] = { mReds[0]->GetColor(),mBlues[0]->GetColor() };
	//�ԐF�̕`��
	for (auto& red : mReds)
	{
		mRender->Render(context, red->GetMesh(), red->GetWorld(), red->GetColor());
	}
	//�F�̕`��
	for (auto& blur : mBlues)
	{
		mRender->Render(context, blur->GetMesh(), blur->GetWorld(), blur->GetColor());
	}
	//�V������������I�u�W�F�N�g�����ŕ`��
	mEditor->EditorCreateObjImageRender(context, mMeshs.at(mEditor->GetCreateData().mObjType).get(), mRender.get(), color[mEditor->GetCreateData().mColorType]);
}
/***************************���x�}�b�v�̕`��*********************************/
void StageManager::RenderVelocity(ID3D11DeviceContext* context, const int stageState)
{
	mRender->VelocityBegin(context);
	for (auto& stage : mStageObjs)
	{
		mRender->VelocityRender(context, stage->GetMesh(), stage->GetWorld(), stage->GetBeforeWorld(), stage->GetColor());
		//���[���h�s��̕ێ�
		stage->SetBeforeWorld(stage->GetWorld());
	}
	mRender->VelocityEnd(context);
}
/*****************************�V���h�E�}�b�v�̕`��*******************************/
void StageManager::RenderShadow(ID3D11DeviceContext* context)
{
	mRender->ShadowBegin(context);
	for (auto& stage : mStageObjs)
	{
		mRender->ShadowRender(context, stage->GetMesh(), stage->GetWorld(), stage->GetColor());
		//���[���h�s��̕ێ�
		stage->SetBeforeWorld(stage->GetWorld());
	}
	mRender->ShadowEnd(context);
}
/****************************�~�j�}�b�v�̕`��************************************/
void StageManager::SidoViewRender(ID3D11DeviceContext* context)
{
	if (!mEditor->GetEditorFlag())return;
	FrameBuffer* frame = mEditor->GetStageSidoViewBuffer();
	frame->Clear(context, 0.5f, 0.5f, 0.5f, 1);
	frame->Activate(context);
	mRender->Begin(context, mEditor->GetCamera()->GetView(), mEditor->GetCamera()->GetProjection());
	//�ԐF�̕`��
	for (auto& red : mReds)
	{
		mRender->Render(context, red->GetMesh(), red->GetWorld(), red->GetColor());
	}
	//�F�̕`��
	for (auto& blur : mBlues)
	{
		mRender->Render(context, blur->GetMesh(), blur->GetWorld(), blur->GetColor());
	}
	mRender->End(context);
	frame->Deactivate(context);
	mEditor->SidoViewRender(context);
}

