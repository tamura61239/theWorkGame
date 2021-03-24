#include "light.h"
#include "misc.h"
#include"camera_manager.h"
#include"file_function.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif


Light::Light()
{
	ZeroMemory(mPointLight, sizeof(PointLight) * POINTMAX);
	ZeroMemory(mSpotLight, sizeof(SpotLight) * SPOTMAX);
	memset(&mDefLight, 1, sizeof(mDefLight));
	FileFunction::Load(mDefLight, "Data/file/light.bin", "rb");
}

void Light::CreateLightBuffer(ID3D11Device* device)
{
	HRESULT hr = S_OK;

	// create constant buffer
	{
		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.ByteWidth = sizeof(CbLight);
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		hr = device->CreateBuffer(&bufferDesc, nullptr, mCbLight.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		bufferDesc.ByteWidth = sizeof(CbDefLight);
		hr = device->CreateBuffer(&bufferDesc, nullptr, mCbDefLight.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}
}

void Light::ImGuiUpdate()
{
#ifdef USE_IMGUI
	ImGui::Begin("light");
	if (ImGui::CollapsingHeader("default light"))
	{
		float* lightDerection[4] = { &mDefLight.mLightDirection.x,&mDefLight.mLightDirection.y ,&mDefLight.mLightDirection.z ,&mDefLight.mLightDirection.w };
		ImGui::SliderFloat4("light direction", *lightDerection, -1, 1);
		float* lightColor[4] = { &mDefLight.mLightColor.x,&mDefLight.mLightColor.y ,&mDefLight.mLightColor.z ,&mDefLight.mLightColor.w };
		ImGui::ColorEdit4("light color", *lightColor);
		float* ambientColor[4] = { &mDefLight.mAmbientColor.x,&mDefLight.mAmbientColor.y ,&mDefLight.mAmbientColor.z ,&mDefLight.mAmbientColor.w };
		ImGui::ColorEdit4("ambient color", *ambientColor);
	}
	float* skyColor[3] = { &mDefLight.mSkyColor.x,&mDefLight.mSkyColor.y ,&mDefLight.mSkyColor.z };
	ImGui::ColorEdit3("sky color", *skyColor);
	float* groundColor[3] = { &mDefLight.mGroundColor.x,&mDefLight.mGroundColor.y ,&mDefLight.mGroundColor.z  };
	ImGui::ColorEdit3("ground color", *groundColor);

	if (ImGui::Button("save"))FileFunction::Save(mDefLight, "Data/file/light.bin", "wb");
	ImGui::End();
#endif
}


void Light::SetPointLight(int index, VECTOR3F position, VECTOR3F color, float range)
{
	if (index < 0) return;
	if (index >= POINTMAX)return;
	mPointLight[index].index = (float)index;
	mPointLight[index].range = range;
	mPointLight[index].type = 1.0f;
	mPointLight[index].dumy = 0.0f;
	mPointLight[index].position = VECTOR4F(position.x, position.y, position.z, 0);
	mPointLight[index].color = VECTOR4F(color.x, color.y, color.z, 0);

}

void Light::SetSpotLight(int index, VECTOR3F position, VECTOR3F color, VECTOR3F dir, float range, float nearArea, float farArea)
{
	if (index < 0) return;
	if (index >= SPOTMAX)return;

	mSpotLight[index].index = (float)index;
	mSpotLight[index].range = range;
	mSpotLight[index].type = 1.0f;
	mSpotLight[index].nearArea = nearArea;
	mSpotLight[index].farArea = farArea;
	mSpotLight[index].dumy0 = 0.0f;
	mSpotLight[index].dumy1 = 0.0f;
	mSpotLight[index].dumy2 = 0.0f;

	mSpotLight[index].position = VECTOR4F(position.x, position.y, position.z, 0);
	mSpotLight[index].color = VECTOR4F(color.x, color.y, color.z, 0);
	mSpotLight[index].dir = VECTOR4F(dir.x, dir.y, dir.z, 0);

}

void Light::ConstanceLightBufferSetShader(ID3D11DeviceContext* context)
{
	context->PSSetConstantBuffers(3, 1, mCbLight.GetAddressOf());
	context->PSSetConstantBuffers(4, 1, mCbDefLight.GetAddressOf());
	CbLight cbLight;
	memcpy(cbLight.pointLight, mPointLight, sizeof(PointLight) * POINTMAX);
	memcpy(cbLight.spotLight, mSpotLight, sizeof(SpotLight) * SPOTMAX);

	mDefLight.mEyePosition = VECTOR4F(pCameraManager->GetCamera()->GetEye().x, pCameraManager->GetCamera()->GetEye().y, pCameraManager->GetCamera()->GetEye().z, .0f);
	context->UpdateSubresource(mCbLight.Get(), 0, 0, &cbLight, 0, 0);
	context->UpdateSubresource(mCbDefLight.Get(), 0, 0, &mDefLight, 0, 0);
}

