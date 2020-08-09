#include "light.h"
#include "misc.h"
#include"camera_manager.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif


Light::Light()
{
	ZeroMemory(pointLight, sizeof(PointLight) * POINTMAX);
	ZeroMemory(spotLight, sizeof(SpotLight) * SPOTMAX);
	Load();
}

void Light::Load()
{
	FILE* fp;
	if (fopen_s(&fp, "Data/file/light.bin", "rb") == 0)
	{
		fread(&mDefLight, sizeof(CbDefLight), 1, fp);
		fclose(fp);
		return;
	}
	mDefLight.mLightDirection = VECTOR4F(1, 1, 1, 0);
	mDefLight.mLightColor = VECTOR4F(1, 1, 1, 1);
	mDefLight.mAmbientColor = VECTOR4F(1, 1, 1, 1);
	mDefLight.mEyePosition = VECTOR4F(1, 1, 1, 1);
	mDefLight.mSkyColor = VECTOR3F(1, 1, 1);
	mDefLight.mGroundColor = VECTOR3F(1, 1, 1);
}

void Light::Save()
{
	FILE* fp;
	fopen_s(&fp, "Data/file/light.bin", "wb");
	fwrite(&mDefLight, sizeof(CbDefLight), 1, fp);
	fclose(fp);
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

	if (ImGui::Button("save"))Save();
	ImGui::End();
#endif
}


void Light::SetPointLight(int index, VECTOR3F position, VECTOR3F color, float range)
{
	if (index < 0) return;
	if (index >= POINTMAX)return;
	pointLight[index].index = (float)index;
	pointLight[index].range = range;
	pointLight[index].type = 1.0f;
	pointLight[index].dumy = 0.0f;
	pointLight[index].position = VECTOR4F(position.x, position.y, position.z, 0);
	pointLight[index].color = VECTOR4F(color.x, color.y, color.z, 0);

}

void Light::SetSpotLight(int index, VECTOR3F position, VECTOR3F color, VECTOR3F dir, float range, float nearArea, float farArea)
{
	if (index < 0) return;
	if (index >= SPOTMAX)return;

	spotLight[index].index = (float)index;
	spotLight[index].range = range;
	spotLight[index].type = 1.0f;
	spotLight[index].nearArea = nearArea;
	spotLight[index].farArea = farArea;
	spotLight[index].dumy0 = 0.0f;
	spotLight[index].dumy1 = 0.0f;
	spotLight[index].dumy2 = 0.0f;

	spotLight[index].position = VECTOR4F(position.x, position.y, position.z, 0);
	spotLight[index].color = VECTOR4F(color.x, color.y, color.z, 0);
	spotLight[index].dir = VECTOR4F(dir.x, dir.y, dir.z, 0);

}

void Light::ConstanceLightBufferSetShader(ID3D11DeviceContext* context)
{
	context->PSSetConstantBuffers(3, 1, mCbLight.GetAddressOf());
	context->PSSetConstantBuffers(4, 1, mCbDefLight.GetAddressOf());
	CbLight cbLight;
	memcpy(cbLight.pointLight, pointLight, sizeof(PointLight) * POINTMAX);
	memcpy(cbLight.spotLight, spotLight, sizeof(SpotLight) * SPOTMAX);

	mDefLight.mEyePosition = VECTOR4F(pCamera.GetCamera()->GetEye().x, pCamera.GetCamera()->GetEye().y, pCamera.GetCamera()->GetEye().z, .0f);
	context->UpdateSubresource(mCbLight.Get(), 0, 0, &cbLight, 0, 0);
	context->UpdateSubresource(mCbDefLight.Get(), 0, 0, &mDefLight, 0, 0);
}

