#include "gpu_particle_manager.h"
#include"framework.h"
#include"misc.h"
#if TYPE
#include"run_particle.h"
#include"stage_scene_particle.h"
#include"title_particle.h"
#include"title_texture_particle.h"
#include"select_scene_particle.h"
#include"fireworks_particle.h"
#include"respond_particle.h"
#endif
/*****************************************************/
//　　　　　　　　　　生成関数
/*****************************************************/
/*************************バッファの生成****************************/
void GpuParticleManager::CreateBuffer(ID3D11Device* device)
{
	HRESULT hr = S_OK;
	//定数バッファ作成
	{
	}

	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"ANGLE",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"VELOCITY",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"SCALE",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
	};
	mSSceneShader = std::make_unique<DrowShader>(device, "Data/shader/particle_motion_data_render_vs.cso", "Data/shader/particle_motion_data_render_gs.cso", "Data/shader/particle_motion_data_render_ps.cso", inputElementDesc, ARRAYSIZE(inputElementDesc));

}
/*************************タイトルシーンのパーティクル生成****************************/

void GpuParticleManager::CreateTitleBuffer(ID3D11Device* device)
{
	mState = STATE::TITLE;
	CreateBuffer(device);
	mTitleParticle = std::make_unique<TitleParticle>(device);
	mTitleTextureParticle = std::make_unique<TitleTextureParticle>(device);
}
/*************************セレクトシーンのパーティクル生成****************************/

void GpuParticleManager::CreateSelectBuffer(ID3D11Device* device)
{
	mState = STATE::SELECT;
	CreateBuffer(device);
	mSelectSceneParticle = std::make_unique<SelectSceneParticle>(device);
}
/*************************ゲームシーンのパーティクル生成****************************/

void GpuParticleManager::CreateGameBuffer(ID3D11Device* device, PlayerManager* player)
{
	mState = STATE::GAME;
	CreateBuffer(device);
	mRunParticle = std::make_unique<RunParticles>(device, player);
	mStageSceneParticle = std::make_unique<StageSceneParticle>(device);
	mRespondParticle = std::make_unique<RespondParticle>(device, player);
}
/*************************リザルトシーンのパーティクル生成****************************/

void GpuParticleManager::CreateResultBuffer(ID3D11Device* device)
{
	mState = STATE::RESULT;

	CreateBuffer(device);

	mFireworksParticle = std::make_unique<FireworksParticle>(device);
}
/*****************************************************/
//　　　　　　　　　　解放関数
/*****************************************************/

void GpuParticleManager::ClearBuffer()
{
	mRunParticle.reset();
	mTitleParticle.reset();
	mStageSceneParticle.reset();
	mRunParticle.reset();
	mSelectSceneParticle.reset();
	mFireworksParticle.reset();
}

/*****************************************************/
//　　　　　　　　　　更新関数
/*****************************************************/

void GpuParticleManager::Update(float elapsd_time)
{
	ID3D11DeviceContext* context = Framework::Instance().GetDeviceContext().Get();
	switch (mState)
	{
	case TITLE:
		mTitleParticle->Update(elapsd_time, context);
		mTitleTextureParticle->Update(elapsd_time, context);
		break;
	case SELECT:
		mSelectSceneParticle->Update(elapsd_time, context);
		break;
	case GAME:
		if (mRunParticle.get() != nullptr)mRunParticle->Update(context, elapsd_time);
		mStageSceneParticle->Update(context, elapsd_time);
		mRespondParticle->Update(context, elapsd_time);
		break;
	case RESULT:
#if (RESULT_TYPE==0)
		mFireworksParticle->Update(elapsd_time, context);
#else
		mStageSceneParticle->Update(context, elapsd_time);
#endif
		break;
	}
}

/*****************************************************/
//　　　　　　　　　　エディタ関数
/*****************************************************/

void GpuParticleManager::Editor()
{
	switch (mState)
	{
	case TITLE:
		TitleEditor();
		break;
	case SELECT:
		SelectEditor();
		break;
	case GAME:
		GameEditor();
		break;
	case RESULT:
		ResultEditor();
		break;
	}
}
TitleTextureParticle* GpuParticleManager::GetTitleTextureParticle()
{
	return mTitleTextureParticle.get();
}
TitleParticle* GpuParticleManager::GetTitleParticle()
{
	return mTitleParticle.get();
}
RunParticles* GpuParticleManager::GetRunParticle()
{
	return mRunParticle.get();
}
FireworksParticle* GpuParticleManager::GetFireworksParticle()
{
	return mFireworksParticle.get();
}
/*************************タイトルシーンのパーティクルエディタ****************************/

void GpuParticleManager::TitleEditor()
{
#ifdef USE_IMGUI
	static bool selects[2] = { false,false };
	ImGui::Begin("title particles");
	ImGui::Selectable("title scene particle", &selects[0]);
	ImGui::Selectable("title text particle", &selects[1]);
	ImGui::End();
	if (selects[0])
	{
		mTitleParticle->Editor();
	}
	if (selects[1])
	{
		mTitleTextureParticle->Editor();
	}

#endif

}
/*************************セレクトシーンのパーティクルエディタ****************************/

void GpuParticleManager::SelectEditor()
{
#ifdef USE_IMGUI
	static bool selects[2] = { false,false };
	ImGui::Begin("select particles");
	ImGui::Selectable("select scene particle", &selects[0]);
	ImGui::End();
	if (selects[0])
	{
		mSelectSceneParticle->Editor();
	}

#endif

}
/*************************ゲームシーンのパーティクルエディタ****************************/

void GpuParticleManager::GameEditor()
{
#ifdef USE_IMGUI
	static bool selects[3] = { false,false,false };
	ImGui::Begin("game particles");
	ImGui::Selectable("run particle", &selects[0]);
	ImGui::Selectable("stage scene particle", &selects[1]);
	ImGui::Selectable("respond particle", &selects[2]);
	ImGui::End();
	if (selects[0])
	{
		if (mRunParticle.get() != nullptr)mRunParticle->Editor();
	}
	if (selects[1])
	{
		mStageSceneParticle->Editor();
     }
	if (selects[2])
	{
		mRespondParticle->Editor();
	}
#endif

}
/*************************リザルトシーンのパーティクルエディタ****************************/

void GpuParticleManager::ResultEditor()
{
#if (RESULT_TYPE==0)
	mFireworksParticle->Editor();
#else
#endif
}
/*****************************************************/
//　　　　　　　　　　描画関数
/*****************************************************/

/*************************通常描画****************************/


void GpuParticleManager::Render(ID3D11DeviceContext* context, bool drowMullti)
{
	switch (mState)
	{
	case TITLE:
		mTitleParticle->Render(context);
		mTitleTextureParticle->Render(context);
		break;
	case SELECT:
		mSelectSceneParticle->Render(context);
		break;
	case GAME:
		if (mRunParticle.get() != nullptr)mRunParticle->Render(context);
		mStageSceneParticle->Render(context);
		mRespondParticle->Render(context);
		break;
	case RESULT:
		mFireworksParticle->Render(context);
		break;
	}

}
/*************************速度マップの描画****************************/

void GpuParticleManager::VelocityRender(ID3D11DeviceContext* context)
{
	switch (mState)
	{
	case SELECT:
		mSelectSceneParticle->Render(context);
		break;
	case GAME:
		//if (mRunParticle.get() != nullptr)mRunParticle->Render(context,mSSceneShader.get());
		mStageSceneParticle->Render(context, mSSceneShader.get());
		break;
	case RESULT:
#if (RESULT_TYPE==0)
		mFireworksParticle->Render(context,mSSceneShader.get());
#else
		mStageSceneParticle->Render(context);
#endif
		break;
	}
}

