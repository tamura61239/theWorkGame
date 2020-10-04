#include "scene_title.h"
#include"key_board.h"
#include"scene_manager.h"
#include"gpu_particle_manager.h"
#include"camera_manager.h"
#include"light.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif
SceneTitle::SceneTitle(ID3D11Device* device)
{
	loading_thread = std::make_unique<std::thread>([&](ID3D11Device* device)
		{
			std::lock_guard<std::mutex> lock(loading_mutex);
			pGpuParticleManager.CreateTitleBuffer(device);
			pCamera.CreateCamera(device);
			//pCamera.GetCameraOperation()->SetCameraType(CameraOperation::CAMERA_TYPE::TITLE_CAMERA);
			//pCamera.GetCameraOperation()->Load();
			pCamera.GetCamera()->SetEye(VECTOR3F(0, 0, -200));
			bloom = std::make_unique<BloomRender>(device,1920,1080);
			frameBuffer = std::make_unique<FrameBuffer>(device, 1920, 1080, true, 8, DXGI_FORMAT_R8G8B8A8_UNORM);
			//std::unique_ptr<ModelData>data = std::make_unique<ModelData>("Data/FBX/new_player_anim.fbx");
			//std::shared_ptr<ModelResource>resouce = std::make_shared<ModelResource>(device, std::move(data));
			pLight.CreateLightBuffer(device);

			//obj = std::make_unique<StaticObj>(device, "Data/FBX/Mr.Incredible/Mr.Incredible.fbx");
			//mRender = std::make_unique<MeshRender>(device);
			//character->GetModel()->PlayAnimation(0, true);

			modelRender=std::make_unique<ModelRenderer>(device);
		}, device);
	test = std::make_unique<Sprite>(device, L"Data/image/タイトルテスト.png");
	blend[0] = std::make_unique<blend_state>(device, BLEND_MODE::ADD);
	blend[1] = std::make_unique<blend_state>(device, BLEND_MODE::ALPHA);

}

void SceneTitle::Update(float elapsed_time)
{
	if (IsNowLoading())
	{
		return;
	}
	EndLoading();
#ifdef USE_IMGUI
	pLight.ImGuiUpdate();

	//ImGui::Begin("character");
	//VECTOR3F position = character->GetPosition();
	//VECTOR3F scale = character->GetScale();
	//float angle[3] = { character->GetAngle().x,character->GetAngle().y,character->GetAngle().z };
	//ImGui::InputFloat("position x", &position.x, 1);
	//ImGui::InputFloat("position y", &position.y, 1);
	//ImGui::InputFloat("position z", &position.z, 1);

	//ImGui::InputFloat("scale x", &scale.x, 1);
	//ImGui::InputFloat("scale y", &scale.y, 1);
	//ImGui::InputFloat("scale z", &scale.z, 1);

	//ImGui::SliderFloat3("angle", angle, -3.14f, 3.14f);

	//character->SetPosition(position);
	//character->SetScale(scale);
	//character->SetAngle(VECTOR3F(angle[0],angle[1],angle[2]));
	//ImGui::End();
#endif
	//character->CalculateBoonTransform(elapsed_time);
	//obj->CalculateTransform();
	pCamera.Update(elapsed_time);
	pGpuParticleManager.Update(elapsed_time);
	bloom->ImGuiUpdate();
	if (pKeyBoad.RisingState(KeyLabel::ENTER))
	{
		pSceneManager.ChangeScene(SCENETYPE::GAME);
		pGpuParticleManager.ClearBuffer();

		return;
	}
}

void SceneTitle::Render(ID3D11DeviceContext* context, float elapsed_time)
{
	if (IsNowLoading())
	{
		return;
	}
	EndLoading();
	frameBuffer->Clear(context);
	frameBuffer->Activate(context);
	FLOAT4X4 view = pCamera.GetCamera()->GetView();
	FLOAT4X4 projection = pCamera.GetCamera()->GetProjection();

	pGpuParticleManager.Render(context, view, projection);
	//FLOAT4X4 viewProjection;

	//DirectX::XMStoreFloat4x4(&viewProjection, DirectX::XMLoadFloat4x4(&view) * DirectX::XMLoadFloat4x4(&projection));
	//pLight.ConstanceLightBufferSetShader(context);
	//blend[1]->activate(context);

	//mRender->Begin(context, view, projection);
	//mRender->Render(context, obj->GetMesh(), obj->GetWorld());
	//mRender->End(context);
	//blend[1]->deactivate(context);
	frameBuffer->Deactivate(context);
	blend[0]->activate(context);
	test->Render(context, frameBuffer->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
	bloom->Render(context, frameBuffer->GetRenderTargetShaderResourceView().Get(), true);
	blend[0]->deactivate(context);
}

SceneTitle::~SceneTitle()
{
}
