#include "light_view.h"
#include"light.h"
#include"file_function.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif
/*****************************************************/
//　　　　　　　　　　初期化関数(コンストラクタ)
/*****************************************************/

LightView::LightView(ID3D11Device* device, std::string fileName)
{
	//カメラの生成(ライト視点の)
	mLightCamera = std::make_unique<Camera>(device);
	std::string filePas = "Data/file/" + fileName + ".bin";
	//データをファイルからロード
	FileFunction::Load(mData, filePas.c_str(), "rb");
}
/*****************************************************/
//　　　　　　　　　　更新関数
/*****************************************************/

void LightView::Update(const VECTOR3F& targetPosition, ID3D11DeviceContext* context)
{
	//viewport取得
	D3D11_VIEWPORT shadowMapViewport;
	UINT num_viewports = 1;
	context->RSGetViewports(&num_viewports, &shadowMapViewport);
	//カメラの範囲を設定
	mLightCamera->SetParallelPerspective(shadowMapViewport.Width / mData.viewportMagnification, shadowMapViewport.Height / mData.viewportMagnification, 0.1f, 3000);
	//注視点を設定
	mLightCamera->SetFocus(targetPosition + VECTOR3F(mData.localFocus[0], mData.localFocus[1], mData.localFocus[2]));
	//ライトベクトルを取得
	VECTOR3F lightDir = VECTOR3F(pLight.GetLightDirection().x, pLight.GetLightDirection().y, pLight.GetLightDirection().z);
	//カメラ座標(eye)を設定
	mLightCamera->SetEye(mLightCamera->GetFocus() + lightDir * mData.lightEyeLength);
	//行列計算
	mLightCamera->CalculateParallelMatrix();
}
/*****************************************************/
//　　　　　　　　　　エディタ関数
/*****************************************************/

void LightView::Editor()
{
#ifdef USE_IMGUI
	ImGui::Begin("light view data");
	//eyeの注視点からの距離
	ImGui::InputFloat("light eye length", &mData.lightEyeLength, 10);
	//カメラ範囲のパラメーターを設定
	ImGui::InputFloat("viewport magnification", &mData.viewportMagnification, 0.1f);
	//注視点を取ってきた座標からどれだけずらすかを決める
	ImGui::DragFloat3("local focus", mData.localFocus, 1);
	//セーブするファイルの名前を入力
	ImGui::InputText("file name", mFileName, 256);
	//セーブ
	if (ImGui::Button("save"))
	{
		//ファイルパスを決める
		std::string fName = mFileName;
		std::string filePas = "Data/file/" + fName + ".bin";

		FileFunction::Save(mData, filePas.c_str(), "wb");
		//初期化
		strcpy_s(mFileName, "");
	}
	ImGui::End();
#endif
}


