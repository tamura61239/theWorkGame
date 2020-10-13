#include "stage_select.h"
#include"texture.h"
#include"misc.h"
#include"key_board.h"
#include"gpu_particle_manager.h"
#ifdef USE_IMGUI
#include"imgui.h"
#endif

StageSelect::StageSelect(ID3D11Device* device) :mSelectSceneFlag(true),mSelectNumber(0),mMaxStage(4), mTextColor(1,1,1,1)
{
	HRESULT hr;

	hr = load_texture_from_file(device, L"Data/image/stage.png", mSelect.mSRV.GetAddressOf(), &mSelect.mDesc);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	mSelect.mDrowSize = VECTOR2F(320, 100);
	hr = load_texture_from_file(device, L"Data/image/number.png", mNumber.mSRV.GetAddressOf(), &mNumber.mDesc);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	mNumber.mDrowSize = VECTOR2F(62, 100);
	mDrow = std::make_unique<Sprite>(device);
	mPositionParsent = VECTOR2F(0, 0);
	mSizeParsent = VECTOR2F(1, 1);
	mMaskLeftUp = VECTOR2F(100, 100);
	mMaskSize = VECTOR2F(850, 1000);
	Load();
}

void StageSelect::ImGuiUpdate()
{
#ifdef USE_IMGUI
	ImGui::Begin("stage select data");
	ImGui::Text("mask");
	ImGui::InputFloat("mask position x", &mMaskLeftUp.x, 10);
	ImGui::InputFloat("mask position y", &mMaskLeftUp.x, 10);
	ImGui::InputFloat("mask size x", &mMaskSize.x, 10);
	ImGui::InputFloat("mask size y", &mMaskSize.x, 10);
	ImGui::Text("drow texture");
	float* sizeParsent[2] = { &mSizeParsent.x,&mSizeParsent.y };

	ImGui::SliderFloat2("size parsent", *sizeParsent, 0, 1);
	float* positionParsent[2] = { &mPositionParsent.x,&mPositionParsent.y };
	ImGui::SliderFloat2("position parsent", *positionParsent, 0, 1);
	float* color[4] = { &mTextColor.x,&mTextColor.y ,&mTextColor.z ,&mTextColor.w };
	ImGui::ColorEdit4("text color", *color);
	ImGui::Text("%d", mSelectNumber);
	if (ImGui::Button("save"))
	{
		Save();
	}
	ImGui::End();
#endif
}

void StageSelect::Update(float elapsdTime, StageManager* manager)
{
	Select(manager);
}

void StageSelect::Select(StageManager* manager)
{
	if (pKeyBoad.RisingState(KeyLabel::RIGHT)&& mSelectNumber < mMaxStage - 1)
	{
		mSelectNumber++;
	}
	if (pKeyBoad.RisingState(KeyLabel::LEFT) && mSelectNumber > 0)
	{
		mSelectNumber--;
	}
	if (pKeyBoad.RisingState(KeyLabel::SPACE))
	{
		manager->SetStageNo(mSelectNumber);
		manager->Load();
		mSelectSceneFlag = false;
		pGpuParticleManager.SetState(GpuParticleManager::STATE::GAME);
	}
}

void StageSelect::Move(float elapsdtime)
{
}

void StageSelect::Mask()
{

}

void StageSelect::Render(ID3D11DeviceContext* context)
{
	mDrow->Render(context, mSelect.mSRV.Get(), mMaskLeftUp + mPositionParsent * mMaskSize, mSelect.mDrowSize*mSizeParsent, VECTOR2F(0, 0), mSelect.mDrowSize, 0, mTextColor);
	mDrow->Render(context, mNumber.mSRV.Get(), mMaskLeftUp + mPositionParsent * mMaskSize+VECTOR2F(mSelect.mDrowSize.x,0) * mSizeParsent, mNumber.mDrowSize * mSizeParsent, VECTOR2F(mNumber.mDrowSize.x*mSelectNumber, 0), mNumber.mDrowSize, 0, mTextColor);
}

void StageSelect::Load()
{
	FILE* fp;
	if (fopen_s(&fp, "Data/file/stage_select_scene.bin", "rb") == 0)
	{
		fread(&mMaskLeftUp, sizeof(VECTOR2F), 1, fp);
		fread(&mMaskSize, sizeof(VECTOR2F), 1, fp);
		fread(&mPositionParsent, sizeof(VECTOR2F), 1, fp);
		fread(&mSizeParsent, sizeof(VECTOR2F), 1, fp);
		fread(&mTextColor, sizeof(VECTOR4F), 1, fp);
		fclose(fp);
	}
}

void StageSelect::Save()
{
	FILE* fp;
	fopen_s(&fp, "Data/file/stage_select_scene.bin", "wb");
	{
		fwrite(&mMaskLeftUp, sizeof(VECTOR2F), 1, fp);
		fwrite(&mMaskSize, sizeof(VECTOR2F), 1, fp);
		fwrite(&mPositionParsent, sizeof(VECTOR2F), 1, fp);
		fwrite(&mSizeParsent, sizeof(VECTOR2F), 1, fp);
		fwrite(&mTextColor, sizeof(VECTOR4F), 1, fp);
		fclose(fp);
	}

}
