#include "light_view.h"
#include"light.h"
#include"file_function.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif

LightView::LightView(ID3D11Device* device, std::string fileName)
{
	mLightCamera = std::make_unique<Camera>(device);
	std::string filePas = "Data/file/" + fileName + ".bin";

	FileFunction::Load(mData, filePas.c_str(), "rb");
}

void LightView::Update(const VECTOR3F& targetPosition, ID3D11DeviceContext* context)
{
	D3D11_VIEWPORT shadowMapViewport;
	UINT num_viewports = 1;
	context->RSGetViewports(&num_viewports, &shadowMapViewport);

	mLightCamera->SetParallelPerspective(shadowMapViewport.Width / mData.viewportMagnification, shadowMapViewport.Height / mData.viewportMagnification, 0.1f, 3000);

	mLightCamera->SetFocus(targetPosition + VECTOR3F(mData.localFocus[0], mData.localFocus[1], mData.localFocus[2]));
	VECTOR3F lightDir = VECTOR3F(pLight.GetLightDirection().x, pLight.GetLightDirection().y, pLight.GetLightDirection().z);

	mLightCamera->SetEye(mLightCamera->GetFocus() + lightDir * mData.lightEyeLength);

	mLightCamera->CalculateParallelMatrix();
}

void LightView::Editor()
{
#ifdef USE_IMGUI
	ImGui::Begin("light view data");
	ImGui::InputFloat("light eye length", &mData.lightEyeLength, 10);
	ImGui::InputFloat("viewport magnification", &mData.viewportMagnification, 0.1f);
	ImGui::DragFloat3("local focus", mData.localFocus, 1);
	ImGui::InputText("file name", mFileName, 256);
	if (ImGui::Button("save"))
	{
		std::string fName = mFileName;
		std::string filePas = "Data/file/" + fName + ".bin";

		FileFunction::Save(mData, filePas.c_str(), "wb");

		strcpy_s(mFileName, "");
	}
	ImGui::End();
#endif
}


