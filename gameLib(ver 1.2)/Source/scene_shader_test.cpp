#include "scene_shader_test.h"
#include"camera_manager.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif
#include"scene_manager.h"
#include"light.h"

SceneShaderTest::SceneShaderTest(ID3D11Device* device)
{
	loading_thread = std::make_unique<std::thread>([&](ID3D11Device* device)
	{
		std::lock_guard<std::mutex> lock(loading_mutex);
		staticObjs.push_back(std::make_unique<StaticObj>(device, "Data/FBX/floor01/floor01.fbx"));
		staticObjs.back()->GetMesh()->CreateShader(device, "Data/shader/static_mesh_multi_vs.cso", "Data/shader/static_mesh_multi_ps.cso");
		staticObjs.back()->SetPosition(VECTOR3F(0, -1, 0));
		staticObjs.push_back(std::make_unique<StaticObj>(device, "Data/FBX/Knight/Knight.fbx",SHADER_TYPE::NORMAL,true));
		staticObjs.back()->GetMesh()->CreateShader(device, "Data/shader/static_mesh_multi_vs.cso", "Data/shader/static_mesh_multi_ps.cso");
		staticObjs.back()->SetPosition(VECTOR3F(5, 1, 0));
		staticObjs.back()->SetScale(VECTOR3F(0.1f, 0.1f, 0.1f));
		staticObjs.push_back(std::make_unique<StaticObj>(device, "Data/FBX/Mr.Incredible/Mr.Incredible.fbx"));
		staticObjs.back()->GetMesh()->CreateShader(device, "Data/shader/static_mesh_multi_vs.cso", "Data/shader/static_mesh_multi_ps.cso");
		staticObjs.back()->SetPosition(VECTOR3F(-5, 1, 0));
		staticObjs.back()->SetScale(VECTOR3F(10.f, 10.f, 10.f));
		colorScene = std::make_shared<FrameBuffer>(device, 1920, 1080, true, 1, DXGI_FORMAT_R8G8B8A8_UNORM);
		positionScene = std::make_shared<FrameBuffer>(device, 1920, 1080, true, 1, DXGI_FORMAT_R16G16B16A16_FLOAT);
		normalScene = std::make_shared<FrameBuffer>(device, 1920, 1080, true, 1, DXGI_FORMAT_R16G16B16A16_FLOAT);
		mulltiRenderTargets = std::make_unique<MulltiRenderTargetFunction>();
		mulltiRenderTargets->SetFrameBuffer(colorScene);
		mulltiRenderTargets->SetFrameBuffer(positionScene);
		mulltiRenderTargets->SetFrameBuffer(normalScene);
		render = std::make_unique<MeshRender>(device);
	}, device);
	pCamera.CreateCamera();
	pCamera.GetCamera()->SetEye(VECTOR3F(0, 40, -200));
	renderScenes = std::make_unique<Sprite>(device);
	pLight.CreateLightBuffer(device);
}

void SceneShaderTest::Update(float elapsed_time)
{
	if (IsNowLoading())
	{
		return;
	}
	EndLoading();
#ifdef USE_IMGUI
	ImGui::Begin("ChangScene");
	if (ImGui::Button("endShaderTest"))
	{
		pSceneManager.ChangeScene(SCENETYPE::TITLE);
		ImGui::End();
		return;
	}
	ImGui::End();
	VECTOR4F light = pLight.GetLightDirection();
	ImGui::Begin("light");
	ImGui::SliderFloat("x", &light.x, -1, 1);
	ImGui::SliderFloat("y", &light.y, -1, 1);
	ImGui::SliderFloat("z", &light.z, -1, 1);
	ImGui::SliderFloat("w", &light.w, -1, 1);
	ImGui::End();
	pLight.SetLightDirection(light);

#endif

	for (auto& obj : staticObjs)
	{
		obj->SetAngle(obj->GetAngle() + VECTOR3F(0, DirectX::XMConvertToRadians(3.f), 0));
		obj->CalculateTransform();
	}
	pCamera.Update(elapsed_time);
}

void SceneShaderTest::Render(ID3D11DeviceContext* context, float elapsed_time)
{
	if (IsNowLoading())
	{
		return;
	}
	mulltiRenderTargets->Clear(context);
	mulltiRenderTargets->Activate(context);
	render->Begin(context, pLight.GetLightDirection(), pCamera.GetCamera()->GetView(), pCamera.GetCamera()->GetProjection());
	for (auto& obj : staticObjs)
	{
		render->Render(context, obj->GetMesh(), obj->GetWorld());
	}
	render->End(context);
	mulltiRenderTargets->Deactivate(context);
	renderScenes->Render(context, colorScene->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(960, 540), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
	renderScenes->Render(context, positionScene->GetRenderTargetShaderResourceView().Get(), VECTOR2F(960, 0), VECTOR2F(960, 540), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
	renderScenes->Render(context, normalScene->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 540), VECTOR2F(960, 540), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
}
